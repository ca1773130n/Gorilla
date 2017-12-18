#pragma once

#include "ou_thread.h"
#include "GRenderer.h"
#include "bitmask.h"
#include "HistoryBar.h"
#include <map>

#define THREAD_POOL_SIZE		128
#define BUFFER_SIZE				81920
#define PORT_NUM				1555

using namespace openutils;

class GServerThread;
class CGServer : public Thread 
{
private:
	BOOL				m_bConnected;
	int					m_nCount;
	SOCKET				m_iSock;
	char				m_szBuf[BUFFER_SIZE];	
	USHORT				m_iPort;
	CGRenderer*			m_pRender;
	BOOL				m_bRestartThread;

	Bitmask				m_BitmaskThreads;
	GServerThread*		m_pWorkers;

	Bitmask*			m_pBitmaskLightsInBaseTex;
	Bitmask*			m_pBitmaskTemp;
	CHistoryBar*		m_pHistoryBar;

public:

	CGServer( VOID );
	~CGServer( VOID );

	CGServer( int n, CGRenderer* pRender, CRITICAL_SECTION* pCS );

	VOID Initialize( CGRenderer* pRender, CHistoryBar* pHistoryBar );
	VOID run();

	BOOL Connected( VOID )					{ return m_bConnected; }

	BOOL GetRestartThread( VOID )			{ return m_bRestartThread; }

	VOID SetRestartThread( BOOL bRestart )	{ m_bRestartThread = bRestart; }

	int FindFreeThread( VOID );

	GServerThread* GetFreeThread( VOID );
};

class GServerThread : public Thread
{
private:

	int					m_ID;
	SOCKET				m_iSock;
	char				m_szMessage[BUFFER_SIZE];	
	USHORT				m_iPort;
	CGRenderer*			m_pRender;
	map<string, int>*	m_pLightsMap;
	Bitmask*			m_pBitmaskThreads;
	Bitmask*			m_pBitmaskLightsInBaseTex;
	Bitmask*			m_pBitmaskTemp;

public:

	GServerThread( VOID ) {}
	~GServerThread( VOID ) {}

	VOID SetThread( int numid, SOCKET iSock, USHORT iPort, CGRenderer* pRender, map<string, int>* lightsMap, Bitmask* pBitmaskThreads, Bitmask* pBitmask, Bitmask* pBitmaskTemp, CRITICAL_SECTION* pCS ) 
	{
		m_ID = numid; 
		m_iSock = iSock;
		m_iPort = iPort;
		m_pRender =  pRender;	
		m_pLightsMap = lightsMap;
		m_pBitmaskThreads = pBitmaskThreads;
		m_pBitmaskLightsInBaseTex = pBitmask;
		m_pBitmaskTemp = pBitmaskTemp;
		m_pCS = pCS;
	}
	VOID SetWork( char* szMessage )
	{
		strcpy(m_szMessage, szMessage);
		if( m_szMessage[strlen(m_szMessage) - 1] == ' ' ) m_szMessage[strlen(m_szMessage) - 1] = '\0';
	}

	VOID run();
};