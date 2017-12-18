#include "stdafx.h"
#include "GServer.h"
#include "MainFrm.h"
#include "GGlobal.h"
//#define GDEBUG(x) { sprintf(g_szOutputBuffer, x); m_pHistoryBar->AddLog(CString(g_szOutputBuffer)); }

extern CRITICAL_SECTION g_cs;

//char g_szOutputBuffer[BUFFER_SIZE];
int g_LightIndex;

char* strtok_r(char *s1, const char *s2, char **lasts)
{
	char *ret;

	if (s1 == NULL)
		s1 = *lasts;
	while(*s1 && strchr(s2, *s1))
		++s1;
	if(*s1 == ' ')
		return NULL;
	ret = s1;
	while(*s1 && !strchr(s2, *s1))
		++s1;
	if(*s1)
		*s1++ = ' ';
	*lasts = s1;
	return ret;
}


CGServer::CGServer( VOID )
{	
}

CGServer::CGServer( int n, CGRenderer* pRender, CRITICAL_SECTION* pCS )
{
	m_bConnected = FALSE;
	m_bRestartThread = FALSE;
	m_iPort = PORT_NUM;

	m_nCount = n;
	m_pRender = pRender;
	m_pCS = pCS;

	m_BitmaskThreads.Allocate(THREAD_POOL_SIZE);

	InitializeCriticalSection(&g_cs);
}

CGServer::~CGServer( VOID )
{	
	closesocket( m_iSock );
	WSACleanup(); 

	GDEBUG("서버 종료.\n");
}

VOID CGServer::Initialize( CGRenderer* pRender, CHistoryBar* pHistoryBar )
{
	m_bConnected = FALSE;
	m_bRestartThread = FALSE;
	m_iPort = PORT_NUM;

	m_nCount = 50;
	m_pRender = pRender;
	m_pCS = &g_cs;
	m_pHistoryBar = pHistoryBar;
	
	m_BitmaskThreads.Allocate(THREAD_POOL_SIZE);
}

VOID 
CGServer::run()
{	
	SOCKET listen_s;
	WSADATA wsaData;
	SOCKADDR_IN saddr, from;
	int fromlen;

	/*
	m_BitmaskThreads.AllSet();
	m_BitmaskThreads.BitClear(32);
	m_BitmaskThreads.BitClear(70);
	m_BitmaskThreads.BitClear(130);
	GDEBUG("%d\n", m_BitmaskThreads.FindFirst0Bit(0));
	GDEBUG("%d\n", m_BitmaskThreads.FindFirst0Bit(32));
	GDEBUG("%d\n", m_BitmaskThreads.FindFirst0Bit(33));
	GDEBUG("%d\n", m_BitmaskThreads.FindFirst0Bit(128));
	GDEBUG("\n\n%d\n", m_BitmaskThreads.CountZeroBit());
	*/

	// WinSock 초기화
	if(WSAStartup(MAKEWORD(1,1), &wsaData)) 
	{
		GDEBUG("Error\n");
		WSACleanup();
	}

	// 대기용 리슨 소켓을 연다.
	listen_s = socket(AF_INET, SOCK_STREAM, 0); 
	if(listen_s < 0)
	{
		GDEBUG("소켓 생성 실패.\n"); 
		WSACleanup();
	}
	else
	{
		GDEBUG("소켓 생성 성공!\n");
	}
	memset(&saddr, 0, sizeof(SOCKADDR_IN));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(m_iPort);
	saddr.sin_addr.s_addr = INADDR_ANY;

	// 소켓에 이름을 붙인다.
	if(bind(listen_s, (struct sockaddr *)&saddr, sizeof(saddr)) == SOCKET_ERROR) 
	{
		GDEBUG("bind Error\n");
		closesocket(listen_s);
		WSACleanup();
	}
	else
	{
		GDEBUG("bind 함수 성공!\n");
	}

	// 소켓을 리슨 상태로 지정한다.
	if(listen(listen_s, 0) == SOCKET_ERROR) 
	{
		GDEBUG("listen Error\n");
		closesocket(listen_s);   
		WSACleanup();
	}
	else GDEBUG("listen 함수 성공!\naccept 함수로 대기합니다.\n");

	// 클라이언트의 접속을 기다린다.
	fromlen = (int)sizeof(from);
	m_iSock = accept(listen_s, (SOCKADDR *)&from, &fromlen); 
	if(m_iSock == INVALID_SOCKET)
	{
		GDEBUG("accept Error\n");
		closesocket(listen_s);    
		WSACleanup();
	}
	else
	{
		GDEBUG("접속.\n");
		GDEBUG("accept 함수 성공!\n대화를 시작합니다.\n\n");
	}
	closesocket(listen_s); 

	m_bConnected = TRUE;
	m_pRender->SetRenderStart();

	int i, j, nRcv;
	BOOL geta = FALSE, getb = FALSE, getc = FALSE;
	BOOL bSelectionChanged = FALSE;

	map<string, int>::iterator iter; 
	map<string, int> lightsMap;

	char* token = NULL;
	char* token2 = NULL;

	GCamera* pCurCamera = m_pRender->GetCurrentCam();
	vector<GLight*>& pLights = m_pRender->GetLights();
	GLight* pLight;

	// bit mask
	m_pBitmaskLightsInBaseTex = m_pRender->GetBitmaskLightsInBaseTex();
	m_pBitmaskTemp = m_pRender->GetBitmaskLink();

	// thread pool
	int iFreeThread;
	m_pWorkers = new GServerThread[THREAD_POOL_SIZE];

	for(int i=0; i < THREAD_POOL_SIZE; i++)
	{
		m_pWorkers[i].SetThread(i, m_iSock, m_iPort, m_pRender, &lightsMap, &m_BitmaskThreads, m_pBitmaskLightsInBaseTex, m_pBitmaskTemp, m_pCS);
		m_pWorkers[i].start();
		m_pWorkers[i].setPriority(P_HIGHEST);
	}


	//***********************************************************************/
	// Server Thread Main Loop                                                                      

	while( 1 )
	{
		nRcv = recv( m_iSock, m_szBuf, sizeof(m_szBuf) - 1, 0 ); 
		if( nRcv == SOCKET_ERROR )
		{
			GDEBUG( "recv 오류 발생.\n" );
			break;
		}

		m_szBuf[nRcv] = '\0';
		if( strcmp(m_szBuf, "c_end") == 0 )
		{
			GDEBUG( "클라이언트가 연결을 끊었습니다.\n" );
			m_bRestartThread = true;
			break;
		}
		//GDEBUG( m_szBuf );

		token = strtok(m_szBuf, "\\");

		while( token )
		{
			if( !strncmp("sync", token, 4) )
			{
				//m_pRender->MakeSamplePoints(0.001);

				int light_num, light_type;

				// 전체 조명 수
				token = strtok_r(token, " ", &token2);
				token = strtok_r(NULL," ", &token2);
				sscanf( token, "%d", &light_num );

				for(j = 0; j < pLights.size(); j++)
					delete pLights[j];
				pLights.clear();

				//pLights.reserve( light_num );

				if( m_pBitmaskLightsInBaseTex->GetTotalBit() < light_num )
					m_pBitmaskLightsInBaseTex->Reallocate(light_num, 1);

				GLight* pNewLight = 0;
				lightsMap.clear();		

				g_LightIndex = 0;

				m_pBitmaskLightsInBaseTex->AllSet();
				m_pBitmaskTemp->AllSet();
			}
			else
			{
				// 쓰레드 실행
				do 
				{
					iFreeThread = FindFreeThread();
					if( iFreeThread < THREAD_POOL_SIZE )
					{
						m_pWorkers[iFreeThread].SetWork(token);
						m_BitmaskThreads.BitSet(iFreeThread);
						m_pWorkers[iFreeThread].resume();
					}			
					else
						sleep(50);
				}while( iFreeThread >= THREAD_POOL_SIZE );
			}		
			token = strtok(NULL, "\\");
			//if( !strncmp(token, "GOR_EOM", 7)) token = strtok_r(NULL, " ", &token2);
		}
	}

	// Server Thread Main Loop                                                                      
	//***********************************************************************/
}

int
CGServer::FindFreeThread( VOID )
{
	return m_BitmaskThreads.FindFirst0Bit(0);	
}

int g_iCamPacketRcv = 0;

VOID
GServerThread::run( VOID )
{
	while(1) {
		m_pBitmaskThreads->BitClear(m_ID);
		suspend();

		UINT i, j;
		BOOL geta = FALSE, getb = FALSE, getc = FALSE;
		BOOL bSelectionChanged = FALSE;

		map<string, int>::iterator iter; 	

		char* token = NULL;
		char* token2 = NULL;

		GCamera* pCurCamera = m_pRender->GetCurrentCam();
		vector<GLight*>& pLights = m_pRender->GetLights();
		GLight* pLight;

		UINT iRenderWidth, iRenderHeight;
		iRenderWidth = m_pRender->GetWidth();
		iRenderHeight = m_pRender->GetHeight();

		// 토큰		
		token = strtok(m_szMessage," ");

		while( token != NULL )
		{			
			if( !strcmp(token, "caminit") )
			{
				pCurCamera->Init( &token );

				geta = true;
				continue;
			}
			if( !strcmp(token, "camera") )
			{
				pCurCamera->Setup( &token, iRenderWidth, iRenderHeight );
				m_pRender->SetCameraChanged( TRUE );
				if( ++g_iCamPacketRcv == 3 ) 
					m_pRender->SetFirstRender( TRUE );

				getb = true;
				continue;
			}
			if( !strcmp(token, "link") )
			{
				/*
				for( UINT i=0; i < pLights.size(); i++ )
				{				
				token = strtok( NULL, " " );
				iter = m_pLi^ghtsMap->find( token );
				if( iter == m_pLightsMap->end() || !strcmp(token, "GOR_EOM") ) return;
				int match  = iter->second;	

				pLights[match]->ParseLinkData(this, &token );
				}			
				m_pRender->SetCacheDirty( TRUE );
				*/

				//m_pRender->Unlock();
				continue;
			}
			if( !strcmp(token, "light") )
			{
				int light_num, light_type;

				token = strtok( NULL, " " );

				// 라이트 종류					
				sscanf( token, "%d", &light_type );					

				token = strtok( NULL, " " );

				GLight* pNewLight;
				switch( light_type )
				{
				case MAYA_AMBIENT : 
					pNewLight = new GAmbientLight( &token, light_type );

					break;
				case MAYA_DIRECTIONAL :
					pNewLight = new GDirectionalLight( &token, light_type );

					break;
				case MAYA_POINT : 
					pNewLight = new GPointLight( &token, light_type );

					break;
				case MAYA_SPOT :
					pNewLight = new GSpotLight( &token, light_type );

					break;
				case MAYA_AREA :
					break;
				case MAYA_VOLUME :
					break;
				default :
					break;
				}					

				//pNewLight->ParseLinkData(this, &token );

				pair<string, int> newPair( pNewLight->GetName(), g_LightIndex++ );
				m_pLightsMap->insert( newPair );
				pLights.push_back( pNewLight );			

				pNewLight->InBaseTex(TRUE);

				pNewLight->ParseLinkData(this, &token );

				m_pRender->SetCacheDirty( TRUE );
				getc = true;
				continue;
			}
			if( !strcmp(token,"mod") )
			{				
				token = strtok(NULL," ");

				string tempstring;
				int match;			

				while( token ) 
				{	
					iter = m_pLightsMap->find( token );
					if( iter == m_pLightsMap->end() || !strcmp(token, "\\") ) break;
					match  = iter->second;	
					pLight = pLights[match];

					pLight->Modify(&token);

					m_pBitmaskTemp->BitInvert(match);

					token = strtok(NULL, " ");
				}
				if( !m_pBitmaskTemp->IsAllSet() )
					m_pRender->SetCacheDirty( TRUE );

				continue;		
			}


			token = strtok(NULL, " ");
		}		

		if(!m_pRender->IsRenderStart())
		{
			m_pRender->SetRenderStart();
		}
		m_pRender->SetNeedRedraw( true );
	}
}