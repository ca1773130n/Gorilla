// GRealtimeViewer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GRealtimeViewer.h"
#include "MainFrm.h"

#include "GRealtimeViewerDoc.h"
#include "GRealtimeViewerView.h"
#include "GGlobal.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#include "girl.h"
#include "api.h"
#include "error.h"
#include "osfunc.h"


#include <boost/tokenizer.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace girl;
namespace po = boost::program_options;

// The one and only CGRealtimeRenderApp object
CGRealtimeViewerApp theApp;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* \brief DXUT 콜백 함수들. 

DXUT 기본 콜백 함수들로 DXUT 초기화시에 등록하는 함수들이다.
여기서는 단순히 메인 클래스의 같은 이름의 핸들러 함수로 연결시킨다. 이유는 메인클래스 멤버들을 편하게 사용하기 위함.
메인 클래스 멤버 자체를 콜백함수로 사용하는것도 가능하나 이 방법이 더 낫다.
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
bool CALLBACK 
IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
	return theApp.GetRenderer().IsDeviceAcceptable( pCaps, AdapterFormat, BackBufferFormat, bWindowed, pUserContext );
}

bool CALLBACK 
ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
	return theApp.GetRenderer().ModifyDeviceSettings( pDeviceSettings, pUserContext );
}

HRESULT CALLBACK 
OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	return theApp.GetRenderer().OnCreateDevice( pd3dDevice, pBackBufferSurfaceDesc, pUserContext );
}

HRESULT CALLBACK 
OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	return theApp.GetRenderer().OnResetDevice( pd3dDevice, pBackBufferSurfaceDesc, pUserContext );
}

void CALLBACK 
OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	theApp.GetRenderer().OnFrameMove( fTime, fElapsedTime, pUserContext );
}

void CALLBACK 
OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{	
	theApp.GetRenderer().OnFrameRender( pd3dDevice, fTime, fElapsedTime, pUserContext );
}

LRESULT CALLBACK 
MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{	
	return theApp.GetRenderer().MsgProc( hWnd, uMsg, wParam, lParam, pbNoFurtherProcessing, pUserContext );
}

void CALLBACK 
OnLostDevice( void* pUserContext )
{
	theApp.GetRenderer().OnLostDevice( pUserContext );
}

void CALLBACK 
OnDestroyDevice( void* pUserContext )
{
	theApp.GetRenderer().OnDestroyDevice( pUserContext );
}

/// CustomUI 이벤트 핸들러. 
void CALLBACK 
OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
	theApp.GetRenderer().OnGUIEvent( nEvent, nControlID, pControl, pUserContext );
}	
*/

bool CALLBACK
IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType, DXGI_FORMAT BufferFormat, bool bWindowed, VOID* pUserContext )
{
	return theApp.GetRenderer().IsDeviceAcceptable( Adapter, Output, DeviceType, BufferFormat, bWindowed, pUserContext );
}

HRESULT CALLBACK
OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBufferSurfaceDesc, VOID* pUserContext )
{
	return theApp.GetRenderer().OnCreateDevice( pd3dDevice, pBufferSurfaceDesc, pUserContext );
}

HRESULT CALLBACK
OnD3D10ResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBufferSurfaceDesc, VOID* pUserContext )
{
	return theApp.GetRenderer().OnResizedSwapChain( pd3dDevice, pSwapChain, pBufferSurfaceDesc, pUserContext );
}

VOID CALLBACK		
OnD3D10ReleasingSwapChain( VOID* pUserContext )
{
	return theApp.GetRenderer().OnReleasingSwapChain( pUserContext );
}

VOID CALLBACK	
OnD3D10DestroyDevice( VOID* pUserContext )
{
	return theApp.GetRenderer().OnDestroyDevice( pUserContext );
}

VOID CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	return theApp.GetRenderer().OnFrameMove( fTime, fElapsedTime, pUserContext );
}

VOID CALLBACK	
OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, VOID* pUserContext )
{
	return theApp.GetRenderer().OnFrameRender( pd3dDevice, fTime, fElapsedTime, pUserContext );
}

LRESULT CALLBACK
MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, VOID* pUserContext )
{
	return theApp.GetRenderer().MsgProc( hWnd, uMsg, wParam, lParam, pbNoFurtherProcessing, pUserContext );
}

VOID CALLBACK	
OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, VOID* pUserContext )
{
	return theApp.GetRenderer().OnKeyboard( nChar, bKeyDown, bAltDown, pUserContext );
}

VOID CALLBACK	
OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, VOID* pUserContext )
{
	return theApp.GetRenderer().OnGUIEvent( nEvent, nControlID, pControl, pUserContext );
}

bool CALLBACK	
ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, VOID* pUserContext )
{
	return theApp.GetRenderer().ModifyDeviceSettings( pDeviceSettings, pUserContext );
}

// CGRealtimeViewerApp

BEGIN_MESSAGE_MAP(CGRealtimeViewerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CGRealtimeViewerApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CGRealtimeViewerApp construction

CGRealtimeViewerApp::CGRealtimeViewerApp() :
	CBCGPWorkspace (TRUE /* m_bResourceSmartUpdate */)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// CGRealtimeViewerApp initialization
#include <conio.h>
BOOL CGRealtimeViewerApp::InitInstance()
{
	HANDLE hMutexOneInstance = ::CreateMutex(NULL, TRUE, _T("GRealtimeViewerer"));
	if(hMutexOneInstance!=NULL && ::GetLastError() == ERROR_ALREADY_EXISTS) exit(1);

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("GRealtimeViewerer"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	SetRegistryBase (_T("Settings"));

	// Initialize all Managers for usage. They are automatically constructed
	// if not yet present
	InitContextMenuManager();
	InitKeyboardManager();

	// TODO: Remove this if you don't want extended tooltips:
	InitTooltipManager();

	CBCGPToolTipParams params;
	params.m_bVislManagerTheme = TRUE;

	theApp.GetTooltipManager ()->SetTooltipParams (
		BCGP_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS (CBCGPToolTipCtrl),
		&params);



	srand(time(NULL));

	// Set numeric format to standard to avoid errors when parsing files
	setlocale(LC_NUMERIC, "C");

	girlInit();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CGRealtimeViewerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CGRealtimeViewerView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

	DXUTSetIsInGammaCorrectMode( FALSE );
	DXUTSetCallbackD3D10DeviceAcceptable( IsD3D10DeviceAcceptable );
	DXUTSetCallbackD3D10DeviceCreated( OnD3D10CreateDevice );
	DXUTSetCallbackD3D10SwapChainResized( OnD3D10ResizedSwapChain );
	DXUTSetCallbackD3D10SwapChainReleasing( OnD3D10ReleasingSwapChain );
	DXUTSetCallbackD3D10DeviceDestroyed( OnD3D10DestroyDevice );
	DXUTSetCallbackD3D10FrameRender( OnD3D10FrameRender );
	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackKeyboard( OnKeyboard );
	DXUTSetCallbackFrameMove( OnFrameMove );
	DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
	
	DXUTInit( TRUE, TRUE, L"-forcevsync 0 -forcevsync 0", TRUE ); 
	DXUTSetCursorSettings( TRUE, TRUE ); 

	m_pRenderer = ((CMainFrame*)m_pMainWnd)->GetRenderer();
	m_pServer = ((CMainFrame*)m_pMainWnd)->GetServer();

	//////////////////////////////////////////////////////////////////////////
	// Set the render view client to fit into desired resolution
	CGRealtimeViewerView* pRenderView = (CGRealtimeViewerView*)((CMainFrame*)AfxGetMainWnd())->GetActiveView();

	CRect mainWindowRect, mainFrameRect, clientRect;
	m_pMainWnd->GetWindowRect( &mainWindowRect );
	m_pMainWnd->GetClientRect( &mainFrameRect );
	pRenderView->GetClientRect( &clientRect );

	UINT xDiff, yDiff;	
	xDiff = mainFrameRect.Width() - clientRect.Width();
	yDiff = mainFrameRect.Height() - clientRect.Height();

	int Width = 1024 + 16;
	int Height = 512 + 8;
	if( S_OK != m_pRenderer->Initialize(Width, Height) ) 
	{
		GDEBUG( "Direct3D initialization failed!" );
		exit( 1	);
	}

	pRenderView->CalcWindowRect( &CRect(0, 0, Width, Height) );
	pRenderView->MoveWindow( 0, 0, Width, Height );
	m_pMainWnd->CalcWindowRect( &CRect(mainFrameRect.left, mainFrameRect.top, Width + xDiff, Height + yDiff) );	
	m_pMainWnd->MoveWindow( mainWindowRect.left, mainWindowRect.top, Width + xDiff, Height + yDiff );
	//////////////////////////////////////////////////////////////////////////

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

// CGRealtimeViewerApp message handlers

int CGRealtimeViewerApp::ExitInstance() 
{
//	FreeConsole ();

	BCGCBProCleanUp();

	return CWinApp::ExitInstance();
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };
	CBCGPURLLinkButton m_btnURL;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMPANY_URL, m_btnURL);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CGRealtimeViewerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CGRealtimeViewerApp message handlers


void CGRealtimeViewerApp::PreLoadState ()
{
	GetContextMenuManager()->AddMenu (_T("to Selection"), IDR_MENU_VIEW);
}

bool CGRealtimeViewerApp::ProcessCommandLine()
{
#if 0
	try {
		// allowed only on command line
		po::options_description generic("Generic options");
		generic.add_options()
			("version,v", "Print version string")
			("help", "Produce help message")
			("debug,d", "Enable debug mode")
			;

		// Declare a group of options that will be
		// allowed both on command line and in
		// config file
		po::options_description config("Configuration");
		config.add_options()
			("threads,t", po::value < int >(), "Specify the number of threads that GRealtime will run in parallel.")
			("useserver,u", po::value< std::vector<std::string> >()->composing(), "Specify the adress of a rendering server to use.")
			("serverinterval,i", po::value < int >(), "Specify the number of seconds between requests to rendering servers.")
			("logconsole,l", "Copy the output of the log tab to the console.")
			;

		// Hidden options, will be allowed both on command line and
		// in config file, but will not be shown to the user.
		po::options_description hidden("Hidden options");
		hidden.add_options()
			("input-file", po::value < vector < string > >(), "input file")
			;

#ifdef GIRL_USE_OPENGL
		generic.add_options()
			("noopengl", "Disable OpenGL to display the image")
			;
#else
		hidden.add_options()
			("noopengl", "Disable OpenGL to display the image")
			;
#endif // GIRL_USE_OPENGL

		po::options_description cmdline_options;
		cmdline_options.add(generic).add(config).add(hidden);

		po::options_description config_file_options;
		config_file_options.add(config).add(hidden);

		po::options_description visible("Allowed options");
		visible.add(generic).add(config);

		po::positional_options_description p;

		p.add("input-file", -1);

		po::variables_map vm;
#if wxUSE_UNICODE == 1
		store(po::wcommand_line_parser(wxApp::argc, wxApp::argv).
			options(cmdline_options).positional(p).run(), vm);
#else // ANSI
		store(po::command_line_parser(wxApp::argc, wxApp::argv).
			options(cmdline_options).positional(p).run(), vm);
#endif // Unicode/ANSI

		std::ifstream ifs("girlrender.cfg");
		store(parse_config_file(ifs, config_file_options), vm);
		notify(vm);

		if(vm.count("help")) {
			std::cout << "Usage: girlrender [options] file..." << std::endl;
			std::cout << visible << std::endl;
			return false;
		}

		if(vm.count("version")) {
			std::cout << "GRealtime version " << GIRL_VERSION_STRING << " of " << __DATE__ << " at " << __TIME__ << std::endl;
			return false;
		}

		if(vm.count("logconsole"))
			m_copyLog2Console = true;
		else
			m_copyLog2Console = false;

		if(vm.count("threads")) {
			m_threads = vm["threads"].as < int >();
		} else {
			// Dade - check for the hardware concurrency available
			m_threads = osHardwareConcurrency();
			if (m_threads == 0)
				m_threads = 1;
		}

		if(vm.count("debug")) {
			girlError(GIRL_NOERROR, GIRL_INFO, "Debug mode enabled");
			girlEnableDebugMode();
		}

		int serverInterval;
		if(vm.count("serverinterval")) {
			serverInterval = vm["serverinterval"].as<int>();
			girlSetNetworkServerUpdateInterval(serverInterval);
		} else {
			serverInterval = girlGetNetworkServerUpdateInterval();
		}

		if(vm.count("useserver")) {
			std::stringstream ss;

			std::vector<std::string> names = vm["useserver"].as<std::vector<std::string> >();

			for(std::vector<std::string>::iterator i = names.begin(); i < names.end(); i++) {
				ss.str("");
				ss << "Connecting to server '" <<(*i) << "'";
				girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

				//TODO jromang : try to connect to the server, and get version number. display message to see if it was successfull
				girlAddServer((*i).c_str());
			}

			m_useServer = true;

			ss.str("");
			ss << "Server requests interval:  " << serverInterval << " secs";
			girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());
		} else {
			m_useServer = false;
		}

		if(vm.count("noopengl")) {
			m_openglEnabled = false;
		} else {
#ifdef GIRL_USE_OPENGL
#if wxUSE_GLCANVAS == 1
			m_openglEnabled = true;
#else
			m_openglEnabled = false;
			girlError(GIRL_NOERROR, GIRL_INFO, "GUI: wxWidgets without suppport for OpenGL canvas - will not be used.");
#endif // wxUSE_GLCANVAS
#else
			m_openglEnabled = false;
			girlError(GIRL_NOERROR, GIRL_INFO, "GUI: OpenGL support was not compiled in - will not be used.");
#endif // GIRL_USE_OPENGL
		}

		if(vm.count("input-file")) {
			const std::vector < std::string > &v = vm["input-file"].as < vector < string > >();
			if(v.size() > 1) {
				girlError(GIRL_SYSTEM, GIRL_SEVERE, "More than one file passed on command line : rendering the first one.");
			}

			m_inputFile = wxString(v[0].c_str(), wxConvUTF8);
		} else {
			m_inputFile.Clear();
		}

		return true;
	} catch(std::exception &e) {
		std::cout << e.what() << std::endl;
		return false;
	}
#endif //0

	return true;
}

int CGRealtimeViewerApp::Run()
{
	HRESULT hr;

	BOOL bNFP;
	BOOL bGotMsg;
	MSG  msg;
	msg.message = WM_NULL;
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

	while( WM_QUIT != msg.message  )
	{
		bGotMsg = ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );

		if( bGotMsg )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			if( m_pRenderer && m_pRenderer->IsRenderStart() && m_pRenderer->IsNeedRedraw() ) 
			{				
				DXUTRender3DEnvironment();	
				((CMainFrame *)AfxGetMainWnd())->m_wndInfoBar.SetFPS( DXUTGetFPS() );
			}
			if( m_pServer->GetRestartThread() )
			{
				m_pServer->SetRestartThread( FALSE );
				m_pServer->stop();
				m_pServer->start();
				m_pServer->setPriority( Thread::P_NORMAL );		
			}
		}
	}

	return S_OK;
}