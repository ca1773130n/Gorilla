// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "GRealtimeViewer.h"

#include "MainFrm.h"

#include <boost/bind.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/thread.hpp>
#include <boost/cast.hpp>

#include <sstream>
#include <clocale>

#include "girl.h"
#include "api.h"
#include "error.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <direct.h>
#include <conio.h>

#include "GRealtimeViewerView.h"

#include "OptionPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace girl;

// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CBCGPFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CBCGPFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_VS2008, OnAppLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_VS2008, OnUpdateAppLook)
	ON_REGISTERED_MESSAGE(BCGM_ON_RIBBON_CUSTOMIZE, OnRibbonCustomize)
	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)
	//ON_WM_NCLBUTTONDBLCLK()
	//ON_WM_TIMER()
	//ON_MESSAGE(UM_UPDATE_THREAD, OnUm_UpdateThread)
	//ON_MESSAGE(UM_RESUME_ACTION, OnUm_ResumeAction)
	//ON_REGISTERED_MESSAGE(UM_LOG_MSG, OnUm_LogMsg)
	ON_COMMAND(ID_VIEW_HISTORY, &CMainFrame::OnViewHistory)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HISTORY, &CMainFrame::OnUpdateViewHistory)
	//ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_PAUSE, &CMainFrame::OnBnClickedBtnPause)
	ON_BN_CLICKED(IDC_BTN_STOP, &CMainFrame::OnBnClickedBtnStop)
	ON_COMMAND(ID_FILE_OPENSCENE, &CMainFrame::OnFileOpenscene)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPENSCENE, &CMainFrame::OnUpdateFileOpenscene)
	ON_COMMAND(ID_VIEW_HISTORYBAR, &CMainFrame::OnViewHistorybar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HISTORYBAR, &CMainFrame::OnUpdateViewHistorybar)
	ON_COMMAND(ID_VIEW_INFORMATIONBAR, &CMainFrame::OnViewInformationbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_INFORMATIONBAR, &CMainFrame::OnUpdateViewInformationbar)
	//ON_WM_SIZE()
	//ON_COMMAND(IDM_COPY, &CMainFrame::OnCopy)
	ON_COMMAND(ID_FILE_OPTION, &CMainFrame::OnFileOption)
END_MESSAGE_MAP()

char* ltrim(char *str) { 
	char *s, *p; 
	for (s = p = str; *s && *s <= ' '; s++) ; 
	while ((*p++ = *s++)) ; 
	return str; 
} 

TCHAR g_szMsg[256];
CEvent g_event;
void GRealtimeGuiErrorHandler(int code, int severity, const char *msg)
{
	_tcscpy (g_szMsg, CString(msg));
	g_event.SetEvent ();
}

UINT fnErrorTrigger (LPVOID lpParam)
{
	while (1)
	{
		g_event.Lock ();

		CMainFrame* pFrame = (CMainFrame *)lpParam;
		pFrame->m_wndHistoryBar.AddLog (g_szMsg);

		g_event.Unlock ();

		if( !lstrcmp(g_szMsg, L"Parsing is done.") )
		{
			pFrame->DoPostParsing();
			break;
		}
	}

	return 0;
}

void CMainFrame::DoPostParsing()
{
	m_Renderer.SetupScene();	
	m_Server.start();
}

// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
	m_nAppLook = theApp.GetInt (_T("ApplicationLook"), ID_VIEW_APPLOOK_2007_1);

	m_bOpenGL = TRUE;
	m_bCopy2Console = FALSE;

	m_renderTimer = -1;
	m_statsTimer = -1;
	m_loadTimer = -1;
 	
	int nPreviewBar = -1;
	int nHistoryBar = -1;
	int nRibbon = -1;

	m_engineThread = NULL;
	m_updateThread = NULL;

	m_numThreads = 0;

 	girlErrorHandler(&GRealtimeGuiErrorHandler);

	ChangeRenderState(WAITING);
	m_guiWindowState = SHOWN;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!CreateRibbonBar ())
	{
		TRACE0("Failed to create ribbon bar\n");
		return -1;      // fail to create
	}

	m_wndRibbonBar.RemoveAllCategories();

	{
		if (!m_wndInfoBar.Create(this, IDD_DIALOG_INFO, CBRS_TOP, IDD_DIALOG_INFO))
		{
			TRACE0("Failed to create info bar\n");
			return FALSE;      // fail to create
		}

		if (!m_wndHistoryBar.Create (_T("History"), this, CRect (0, 0, 300, 200),
			TRUE, 
			ID_VIEW_HISTORY,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
		{
			TRACE0("Failed to create history bar\n");
			return FALSE;      // fail to create
		}
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: add your status bar panes here:
	m_wndStatusBar.AddElement (new CBCGPRibbonStatusBarPane (ID_STATUSBAR_PANE1, _T("Information"), TRUE), _T("Information"));
	m_wndStatusBar.AddExtendedElement (new CBCGPRibbonStatusBarPane (ID_STATUSBAR_PANE2, _T("Status"), TRUE), _T("Status"));

	m_wndInfoBar.EnableDocking(CBRS_ALIGN_TOP);
	m_wndHistoryBar.EnableDocking(CBRS_ALIGN_ANY);

	EnableDocking(CBRS_ALIGN_ANY);
	EnableAutoHideBars(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndInfoBar);
	DockControlBar(&m_wndHistoryBar);
	m_wndInfoBar.Init();	

	OnAppLook (m_nAppLook);

	SetRenderThreads (m_wndInfoBar.GetNumOfThread());

	m_Server.Initialize( &m_Renderer, NULL );

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGPFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	
	cs.style &= ~FWS_ADDTOTITLE;
	cs.lpszName = _T("GRealtimeViewerer - ETRI");

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CBCGPFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CBCGPFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers


void CMainFrame::OnAppLook(UINT id)
{
	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);

	m_nAppLook = id;

	CBCGPTabbedControlBar::m_StyleTabWnd = CBCGPTabWnd::STYLE_3D;

	switch (m_nAppLook)
	{
	case ID_VIEW_APPLOOK_2000:
		// enable Office 2000 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager));
		break;

	case ID_VIEW_APPLOOK_XP:
		// enable Office XP look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		// enable Windows XP look (in other OS Office XP look will be used):
		CBCGPWinXPVisualManager::m_b3DTabsXPTheme = TRUE;
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPWinXPVisualManager));
		break;

	case ID_VIEW_APPLOOK_2003:
		// enable Office 2003 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2003));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_2007:
	case ID_VIEW_APPLOOK_2007_1:
	case ID_VIEW_APPLOOK_2007_2:
	case ID_VIEW_APPLOOK_2007_3:
		// enable Office 2007 look:
		switch (m_nAppLook)
		{
		case ID_VIEW_APPLOOK_2007:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_2007_1:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_2007_2:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Silver);
			break;

		case ID_VIEW_APPLOOK_2007_3:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Aqua);
			break;
		}

		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS2005:
		// enable VS 2005 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2005));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS2008:
		// enable VS 2008 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2008));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;
	}

	CBCGPDockManager* pDockManager = GetDockManager ();
	if (pDockManager != NULL)
	{
		ASSERT_VALID (pDockManager);
		pDockManager->AdjustBarFrames ();
	}

	CBCGPTabbedControlBar::ResetTabs ();

	RecalcLayout ();
	RedrawWindow (NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt (_T("ApplicationLook"), m_nAppLook);
}

void CMainFrame::OnUpdateAppLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio (m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::CreateRibbonBar ()
{
	if (!m_wndRibbonBar.Create(this))
	{
		return FALSE;
	}

	// Load panel images:
	m_PanelImages.SetImageSize (CSize (16, 16));
	m_PanelImages.Load (IDB_RIBBON_ICONS);

	// Init main button:
	m_MainButton.SetImage (IDB_RIBBON_MAIN);
	m_MainButton.SetToolTipText (_T("File"));
	m_MainButton.SetText (_T("\nf"));

	m_wndRibbonBar.SetMainButton (&m_MainButton, CSize (45, 45));

	CBCGPRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory (_T("File"), IDB_RIBBON_FILESMALL, IDB_RIBBON_FILELARGE);
	pMainPanel->Add (new CBCGPRibbonButton (ID_FILE_OPENSCENE, _T("&Open..."), 1, 1));
	//pMainPanel->Add (new CBCGPRibbonButton (IDM_COPY, _T("&Copy..."), 1, 1));
	pMainPanel->Add (new CBCGPRibbonButton (ID_FILE_OPTION, _T("&Option..."), 6, 6));
	pMainPanel->Add (new CBCGPRibbonButton (ID_FILE_SAVE, _T("&Save"), 2, 2));
	pMainPanel->Add (new CBCGPRibbonButton (ID_FILE_SAVE_AS, _T("Save &As..."), 3, 3));

	pMainPanel->AddRecentFilesList (_T("Recent Documents"));
	pMainPanel->AddToBottom (new CBCGPRibbonMainPanelButton (ID_APP_EXIT, _T("E&xit"), 8));

	// Add "Scene" category with "Load" panel:
	CBCGPRibbonCategory* pCategory = m_wndRibbonBar.AddCategory (_T("S&tatus"), IDB_RIBBON_FILESMALL, IDB_RIBBON_FILELARGE);

	// Create "Clipboard" panel:
	CBCGPRibbonPanel* pPanelScene = pCategory->AddPanel (_T("Load"), m_PanelImages.ExtractIcon (1));
	{
		CBCGPRibbonButton* pBtnOpen = new CBCGPRibbonButton (ID_FILE_OPEN, _T("&Open Scene\no"), 0, 0);
		pPanelScene->Add (pBtnOpen);
		CBCGPRibbonButton* pBtnSave = new CBCGPRibbonButton (ID_FILE_SAVE, _T("&Save Scene\ns"), 0, 0);
		pPanelScene->Add (pBtnSave);

		pPanelScene->SetKeys (_T("zl"));
	}

	// Add quick access toolbar commands:
	CBCGPRibbonQATDefaultState qatState;
	{
		qatState.AddCommand (ID_FILE_NEW, FALSE);
		qatState.AddCommand (ID_FILE_OPENSCENE);
		//qatState.AddCommand (IDM_COPY);
		qatState.AddCommand (ID_FILE_OPTION);
		qatState.AddCommand (ID_FILE_SAVE);
		qatState.AddCommand (ID_FILE_PRINT_DIRECT);
		qatState.AddCommand (ID_FILE_PRINT_PREVIEW, FALSE);
		qatState.AddCommand (ID_EDIT_UNDO);
		m_wndRibbonBar.SetQuickAccessDefaultState (qatState);
	}

	// Add "Style" button to the right of tabs:
	CBCGPRibbonButton* pStyleButton = new CBCGPRibbonButton (-1, _T("Style\ns"), -1, -1);
	pStyleButton->SetMenu (IDR_THEME_MENU, TRUE /* Right align */);
	m_wndRibbonBar.AddToTabs (pStyleButton);

	// Add "About" button to the right of tabs:
	m_wndRibbonBar.AddToTabs (new CBCGPRibbonButton (ID_APP_ABOUT, _T("\na"), m_PanelImages.ExtractIcon (0)));

	return TRUE;
}

LRESULT CMainFrame::OnRibbonCustomize (WPARAM wp, LPARAM lp)
{
	ShowOptions (0);
	return 1;
}

void CMainFrame::OnToolsOptions()
{
	ShowOptions (0);
}

void CMainFrame::ShowOptions (int nPage)
{
	// Create "Customize" page:
	CBCGPRibbonCustomizePage pageCustomize (&m_wndRibbonBar);

	// Add "popular" items:
	CList<UINT, UINT> lstPopular;

	lstPopular.AddTail (ID_FILE_NEW);
	lstPopular.AddTail (ID_FILE_OPEN);
	lstPopular.AddTail (ID_FILE_SAVE);
	lstPopular.AddTail (ID_FILE_PRINT_PREVIEW);
	lstPopular.AddTail (ID_FILE_PRINT_DIRECT);
	lstPopular.AddTail (ID_EDIT_UNDO);

	pageCustomize.AddCustomCategory (_T("Popular Commands"), lstPopular);

	// Add hidden commands:
	CList<UINT,UINT> lstHidden;
	m_wndRibbonBar.GetItemIDsList (lstHidden, TRUE);

	pageCustomize.AddCustomCategory (_T("Commands not in the Ribbon"), lstHidden);

	// Add all commands:
	CList<UINT,UINT> lstAll;
	m_wndRibbonBar.GetItemIDsList (lstAll);

	pageCustomize.AddCustomCategory (_T("All Commands"), lstAll);

	// Create property sheet:
	CBCGPPropertySheet propSheet (_T("Options"), this, nPage);
	propSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	propSheet.SetLook (CBCGPPropertySheet::PropSheetLook_List, 124 /* List width */);

	propSheet.AddPage (&pageCustomize);

	if (propSheet.DoModal() != IDOK)
	{
		return;
	}
}

void CMainFrame::EngineThread ()
{
	string sfile;
	char szTemp[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, m_sFilePath.GetBuffer (m_sFilePath.GetLength ()), -1, szTemp, MAX_PATH, 0, 0);
	sfile = szTemp;
	m_sFilePath.ReleaseBuffer();

	boost::filesystem::path fullPath(boost::filesystem::initial_path());
	fullPath = boost::filesystem::system_complete(boost::filesystem::path(sfile.c_str(), boost::filesystem::native));

	chdir(fullPath.branch_path().string().c_str());

	CString sScene, sScenePath;
	sScene.Format (L"Scene: %s", fullPath.leaf().c_str());
	m_wndHistoryBar.AddLog (sScene);
	sScenePath.Format (L"Current path: %s", fullPath.branch_path().string().c_str());
	m_wndHistoryBar.AddLog (sScenePath);

	ParseFile(fullPath.leaf().c_str());

	KillTimer (m_renderTimer);
	KillTimer (m_statsTimer);
	/*
	if(!girlStatistics("sceneIsReady")) {
		::MessageBox (NULL, L"Scene file parse error", L"Error", MB_OK|MB_ICONASTERISK);
		ChangeRenderState(FINISHED);
		girlWait();
	} else {
		girlWait();

		//girlError(GIRL_NOERROR, GIRL_INFO, "Rendering done.");
		m_wndHistoryBar.AddLog (_T("Rendering done"));
		m_wndInfoBar.SetProgressDone ();

		ChangeRenderState(FINISHED);

		KillTimer (m_renderTimer);

		if (m_updateThread == NULL && girlStatistics ("sceneIsReady") && 
			(m_guiWindowState == SHOWN || m_guiRenderState == FINISHED))
		{
			//luxError(LUX_NOERROR, LUX_INFO, "GUI: Updating framebuffer...");
			//m_wndHistoryBar.AddLog (_T("GUI: Updating framebuffer..."));
			SetStr2Status(L"Tonemapping...");
			m_updateThread = new boost::thread(boost::bind(&CMainFrame::UpdateThread, this));
		}

		KillTimer (m_statsTimer);

		if(girlStatistics("sceneIsReady"))
		{
			UpdateStatistics();

			if(m_guiRenderState == STOPPING && m_samplesSec == 0.0)
			{
				// Render threads stopped, do one last render update
				//girlError(LUX_NOERROR, LUX_INFO, "GUI: Updating framebuffer...");
				m_wndHistoryBar.AddLog (_T("GUI: Updating framebuffer..."));
				SetStr2Status(L"Tonemapping...");
				m_updateThread = new boost::thread(boost::bind(&CMainFrame::UpdateThread, this));
				KillTimer (m_statsTimer);
				girlPause();
				//girlError(LUX_NOERROR, LUX_INFO, "Rendering stopped by user.");
				m_wndHistoryBar.AddLog (_T("Rendering stopped by user."));
				ChangeRenderState(STOPPED);
			}
		}
	}
	*/

	
}

void CMainFrame::SetRenderThreads(int num)
{
	if(girlStatistics("sceneIsReady")) {
		if(num > m_numThreads) {
			for(; num > m_numThreads; m_numThreads++)
			{
				girlAddThread();
				_cprintf ("girlAddThread called\n");
			}
		} else {
			for(; num < m_numThreads; m_numThreads--)
			{
				girlRemoveThread();
				_cprintf ("girlRemoveThread called\n");
			}
		}
	} else {
		m_numThreads = num;
	}

	_cprintf ("current working thread: %d \n", m_numThreads);
}

void CMainFrame::ChangeRenderState(GRealtimeGuiRenderState state)
{
	switch(state) {
		case WAITING:
			// Waiting for input file. Most controls disabled.
// 			m_render->Enable(ID_RESUMEITEM, false);
// 			m_render->Enable(ID_PAUSEITEM, false);
// 			m_render->Enable(ID_STOPITEM, false);
// 			m_view->Enable(ID_RENDER_COPY, false);
// 			m_renderToolBar->EnableTool(ID_RESUMETOOL, false);
// 			m_renderToolBar->EnableTool(ID_PAUSETOOL, false);
// 			m_renderToolBar->EnableTool(ID_STOPTOOL, false);
// 			m_renderToolBar->EnableTool(ID_RENDER_COPY, false);
// 			m_viewerToolBar->Disable();
			break;
		case RENDERING:
			// Rendering is in progress.
// 			m_render->Enable(ID_RESUMEITEM, false);
// 			m_render->Enable(ID_PAUSEITEM, true);
// 			m_render->Enable(ID_STOPITEM, true);
// 			m_view->Enable(ID_RENDER_COPY, true);
// 			m_renderToolBar->EnableTool(ID_RESUMETOOL, false);
// 			m_renderToolBar->EnableTool(ID_PAUSETOOL, true);
// 			m_renderToolBar->EnableTool(ID_STOPTOOL, true);
// 			m_renderToolBar->EnableTool(ID_RENDER_COPY, true);
// 			m_viewerToolBar->Enable();
			break;
		case STOPPING:
			// Rendering is being stopped.
// 			m_render->Enable(ID_RESUMEITEM, false);
// 			m_render->Enable(ID_PAUSEITEM, false);
// 			m_render->Enable(ID_STOPITEM, false);
// 			m_view->Enable(ID_RENDER_COPY, true);
// 			m_renderToolBar->EnableTool(ID_RESUMETOOL, false);
// 			m_renderToolBar->EnableTool(ID_PAUSETOOL, false);
// 			m_renderToolBar->EnableTool(ID_STOPTOOL, false);
// 			m_renderToolBar->EnableTool(ID_RENDER_COPY, true);
			break;
		case STOPPED:
			// Rendering is stopped.
// 			m_render->Enable(ID_RESUMEITEM, true);
// 			m_render->Enable(ID_PAUSEITEM, false);
// 			m_render->Enable(ID_STOPITEM, false);
// 			m_view->Enable(ID_RENDER_COPY, true);
// 			m_renderToolBar->EnableTool(ID_RESUMETOOL, true);
// 			m_renderToolBar->EnableTool(ID_PAUSETOOL, false);
// 			m_renderToolBar->EnableTool(ID_STOPTOOL, false);
// 			m_renderToolBar->EnableTool(ID_RENDER_COPY, true);
			break;
		case PAUSED:
			// Rendering is paused.
// 			m_render->Enable(ID_RESUMEITEM, true);
// 			m_render->Enable(ID_PAUSEITEM, false);
// 			m_render->Enable(ID_STOPITEM, true);
// 			m_view->Enable(ID_RENDER_COPY, true);
// 			m_renderToolBar->EnableTool(ID_RESUMETOOL, true);
// 			m_renderToolBar->EnableTool(ID_PAUSETOOL, false);
// 			m_renderToolBar->EnableTool(ID_STOPTOOL, true);
// 			m_renderToolBar->EnableTool(ID_RENDER_COPY, true);
			break;
		case FINISHED:
			// Rendering is finished.
// 			m_render->Enable(ID_RESUMEITEM, false);
// 			m_render->Enable(ID_PAUSEITEM, false);
// 			m_render->Enable(ID_STOPITEM, false);
// 			m_view->Enable(ID_RENDER_COPY, true);
// 			m_renderToolBar->EnableTool(ID_RESUMETOOL, false);
// 			m_renderToolBar->EnableTool(ID_PAUSETOOL, false);
// 			m_renderToolBar->EnableTool(ID_STOPTOOL, false);
// 			m_renderToolBar->EnableTool(ID_RENDER_COPY, true);
			break;
	}
	m_guiRenderState = state;
}

void CMainFrame::UpdateThread()
{
	girlUpdateFramebuffer();

	// wxEVT_LUX_TONEMAPPED
	PostMessage (UM_UPDATE_THREAD, 0, 0L);
}

void CMainFrame::UpdateStatistics()
{
	m_samplesSec = girlStatistics("samplesSec");
	int samplesSec = Floor2Int(m_samplesSec);
	int samplesTotSec = Floor2Int(girlStatistics("samplesTotSec"));
	int secElapsed = Floor2Int(girlStatistics("secElapsed"));
	double samplesPx = girlStatistics("samplesPx");
	int efficiency = Floor2Int(girlStatistics("efficiency"));
	int haltspp = girlStatistics("haltspp");

	int secs = (secElapsed) % 60;
	int mins = (secElapsed / 60) % 60;
	int hours = (secElapsed / 3600);

	//wxString stats;
	CString stats;
	stats.Format (_T("%02d:%02d:%02d - %d S/s - %d TotS/s - %.2f S/px - %i%% eff"),
		hours, mins, secs, samplesSec, samplesTotSec, samplesPx, efficiency);
	SetStr2Status (stats);
	
	{
		CString sTime;
		sTime.Format (L"%02dh%02dm%02ds", hours, mins, secs);
		m_wndInfoBar.SetData (sTime, samplesSec, samplesTotSec, haltspp, samplesPx);
 	}

}

/* 
 * 
 * lpszFilePath: file path and name 
 */
void CMainFrame::SetOpenFile(LPCTSTR lpszFilePath, int nTime)
{
	if(m_guiRenderState == RENDERING) {
		if(::MessageBox(this->m_hWnd, _T("Current file is still rendering. Do you want to continue?"), _T(""), MB_YESNO | MB_ICONQUESTION) == IDNO)
			return;
	}

	SetStr2Status(L"Parsing scene ...");

	// Clean up if this is not the first rendering
	if(m_guiRenderState != WAITING)
	{

		if(m_guiRenderState != FINISHED) {
			if(m_updateThread)
				m_updateThread->join();
			girlExit();
			if(m_engineThread)
				m_engineThread->join();
		}
		girlError(GIRL_NOERROR, GIRL_INFO, "Freeing resources.");

		m_wndHistoryBar.AddLog (_T("Freeing resources."));
		girlCleanup();
		ChangeRenderState(WAITING);
	}

	if (nTime != 0)
	{
		m_timeoutTimer = SetTimer (ID_TIMEOUT, (nTime*60)*1000, NULL);
	}

	RenderSceneFile (lpszFilePath);
}

/* 
 * 1. 로그 메세지 모니터링 쓰레드 생성
 * 2. 로딩 에니메이션 표시
 * 3. 메인 쓰레드 생성
 * 4. 업데이트 타이머 생성
 */
void CMainFrame::RenderSceneFile(LPCTSTR filename)
{
	// 1. Creating the thread to monitor the log message from GRealtime core.
	AfxBeginThread (&fnErrorTrigger, this);

	m_sFilePath = filename;

	// 2. Creating the main thread
	m_engineThread = new boost::thread (boost::bind(&CMainFrame::EngineThread, this));

	// 3. Display the loading animation
	/*
	if (m_pLoadingDlg == NULL)
	{
		m_pLoadingDlg = new CLoadingDlg;
		m_pLoadingDlg->Create (IDD_DIALOG_LOAD, this);
		m_pLoadingDlg->ModifyStyle(WS_CAPTION, 0, 0);
		m_pLoadingDlg->CenterWindow ();
		m_pLoadingDlg->ShowWindow (SW_SHOW);
		m_pLoadingDlg->StartAnimation();
	}
	*/
}

int g_nt = 0;
void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	/*
	if (m_renderTimer == nIDEvent)
	{
		if (m_updateThread == NULL && girlStatistics("sceneIsReady") && (m_guiWindowState == SHOWN || m_guiRenderState == FINISHED)) {
			girlError(GIRL_NOERROR, GIRL_INFO, "GUI: Updating framebuffer...");

			SetStr2Status(L"Tonemapping...");
			m_updateThread = new boost::thread(boost::bind(&CMainFrame::UpdateThread, this));
		}
	}
	else if (m_statsTimer == nIDEvent)
	{
		if(girlStatistics("sceneIsReady")) {
			UpdateStatistics();

			if(m_guiRenderState == STOPPING && m_samplesSec == 0.0) {
				// Render threads stopped, do one last render update
				girlError(GIRL_NOERROR, GIRL_INFO, "GUI: Updating framebuffer...");
				
				m_updateThread = new boost::thread(boost::bind(&CMainFrame::UpdateThread, this));
				KillTimer (m_statsTimer);
				girlPause();
				ChangeRenderState(STOPPED);
				girlError(GIRL_NOERROR, GIRL_INFO, "Rendering stopped by user.");
				SetStr2Status(L"Stopped.");
			}
		}
	}
	else if (m_loadTimer == nIDEvent)
	{
		if (girlStatistics("sceneIsReady") || m_guiRenderState == FINISHED)
		{
			KillTimer (m_loadTimer);

			if(girlStatistics("sceneIsReady")) {
				// Scene file loaded
				// Add other render threads if necessary
				int curThreads = 1;
				while(curThreads < m_numThreads)
				{
					girlAddThread();
					curThreads++;
				}

				// Start updating the display by faking a resume menu item click.
				{
					if(m_guiRenderState != RENDERING)
					{
						// Start display update timer
						CGRealtimeViewerView* pView = (CGRealtimeViewerView *)GetActiveView ();
						pView->Reload();

						m_renderTimer = SetTimer (ID_RENDERUPDATE, 1000*girlStatistics("displayInterval"), NULL);
						m_statsTimer = SetTimer (ID_STATSUPDATE, 1000, NULL);
						if(m_guiRenderState == PAUSED || m_guiRenderState == STOPPED) // Only re-start if we were previously stopped
							girlStart();
						if(m_guiRenderState == STOPPED)
							girlSetHaltSamplePerPixel(-1, false, false);
						ChangeRenderState(RENDERING);
					}

				}
			}
		}
	}
	else if (ID_TIMEOUT == nIDEvent)
	{
		KillTimer (m_timeoutTimer);

		doAction (CTL_STOP); // stop

		girlError(GIRL_NOERROR, GIRL_INFO, "Aborted by Timeout");
	}
	*/
	CFrameWnd::OnTimer(nIDEvent);
}
/*
void CMainFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	CBCGPFrameWnd::OnNcLButtonDblClk(nHitTest, point);
}

// wxEVT_LUX_TONEMAPPED
long CMainFrame::OnUm_UpdateThread (WPARAM wParam, LPARAM lParam)
{
	m_updateThread->join();
	delete m_updateThread;
	m_updateThread = NULL;

	SetStr2Status(L"");

	//m_renderOutput->Reload();
	CGRealtimeViewerView* pView = (CGRealtimeViewerView *)GetActiveView ();
	pView->Reload ();

	return 0L;
}

long CMainFrame::OnUm_ResumeAction (WPARAM wParam, LPARAM lParam)
{
	if(m_guiRenderState != RENDERING)
	{
		// Start display update timer
		CGRealtimeViewerView* pView = (CGRealtimeViewerView *)GetActiveView ();
		pView->Reload();

		m_renderTimer = SetTimer (ID_RENDERUPDATE, 1000*girlStatistics("displayInterval"), NULL);
		m_statsTimer = SetTimer (ID_STATSUPDATE, 1000, NULL);
		if(m_guiRenderState == PAUSED || m_guiRenderState == STOPPED) // Only re-start if we were previously stopped
			girlStart();
		if(m_guiRenderState == STOPPED)
			girlSetHaltSamplePerPixel(-1, false, false);
		ChangeRenderState(RENDERING);
	}

	return 0L;
}
*/
LRESULT CMainFrame::OnUm_LogMsg (WPARAM wParam, LPARAM lParam)
{
	std::stringstream ss("");
	ss << boost::posix_time::second_clock::local_time() << ' ';

	CString sMsg;
	sMsg.Format (_T("%s%s"), ss.str().c_str(), (char*)lParam);

//	m_wndHistoryBar.AddLog (sMsg);

	return 0L;
}

void CMainFrame::OnViewHistory()
{
	ShowControlBar (&m_wndHistoryBar,
		!(m_wndHistoryBar.IsVisible ()),
		FALSE, TRUE);
	RecalcLayout ();
}

void CMainFrame::OnUpdateViewHistory(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (m_wndHistoryBar.IsVisible ());
	pCmdUI->Enable (!GetDockManager ()->IsPrintPreviewValid ());
}

/*
void CMainFrame::OnClose()
{
	if (m_guiRenderState != WAITING)
		exit (0);

	CBCGPFrameWnd::OnClose();
}
*/

void CMainFrame::doAction(GRealtimeCtlMode ctlMode)
{
	/*
	if (ctlMode == CTL_PAUSE)	// pause
	{
		if (m_guiRenderState != PAUSED)
		{
			// Stop display update timer
			KillTimer (m_renderTimer);
			KillTimer (m_statsTimer);

			CGRealtimeViewerView* pView = (CGRealtimeViewerView *)GetActiveView ();
			pView->Reset ();

			if(m_guiRenderState == RENDERING)
				girlPause();

			ChangeRenderState(PAUSED);

			SetStr2Status(L"Paused");
		}
	}
	else if (ctlMode == CTL_RESUME) // resume
	{
		if(m_guiRenderState != RENDERING)
		{
			// Start display update timer
			CGRealtimeViewerView* pView = (CGRealtimeViewerView *)GetActiveView ();
			pView->Reload();

			m_renderTimer = SetTimer (ID_RENDERUPDATE, 1000*girlStatistics("displayInterval"), NULL);
			m_statsTimer = SetTimer (ID_STATSUPDATE, 1000, NULL);

			if(m_guiRenderState == PAUSED || m_guiRenderState == STOPPED) // Only re-start if we were previously stopped
				girlStart();
			if(m_guiRenderState == STOPPED)
				girlSetHaltSamplePerPixel(-1, false, false);

			ChangeRenderState(RENDERING);
		}
	}
	else if (ctlMode == CTL_STOP) // stop
	{
		if ((m_guiRenderState == RENDERING) || (m_guiRenderState == PAUSED))
		{
 			KillTimer (m_renderTimer);
			girlSetHaltSamplePerPixel (1, true, true);

			SetStr2Status (L"Stopping ...");
			ChangeRenderState (STOPPING);

		}
	}
	*/
}

void CMainFrame::OnBnClickedBtnPause()
{
	_cprintf ("just clicked 'pause' \n");
}

void CMainFrame::OnBnClickedBtnStop()
{
	_cprintf ("just clicked 'stop' \n");
}

void CMainFrame::OnFileOpenscene()
{
	char* szGorillaPath = getenv( "GorillaPath" );
	TCHAR name_filter[] = L"Go!Rilla Cache (*.girls)|*.girls|All Files (*.*)|*.*||";

	CString lpszGorillaCachePath( szGorillaPath );
	lpszGorillaCachePath += L"\\cache\\";
	//lpszGorillaCachePath = L"c:\\girl\\000001\\";

	CFileDialog dlg( TRUE, L"cache", lpszGorillaCachePath + CString(L"*.girls"), OFN_OVERWRITEPROMPT, name_filter, NULL );
	if (dlg.DoModal () == IDOK)
	{
		ChangeRenderState(FINISHED);
		
		//CGRealtimeViewerView* pView = (CGRealtimeViewerView *)GetActiveView();
		//pView->Reset();

		m_Renderer.OnFileOpen( dlg.GetPathName() );				

		//SetOpenFile (dlg.GetPathName ());

		m_bSceneOpened = TRUE;
	}
}

void CMainFrame::OnUpdateFileOpenscene(CCmdUI *pCmdUI)
{
	if (m_bSceneOpened == TRUE)
		pCmdUI->Enable(FALSE);
}

void CMainFrame::SetStr2Status (CString sMsg)
{
	(m_wndStatusBar.GetElement(0))->SetText (sMsg);
	m_wndStatusBar.RecalcLayout();
	m_wndStatusBar.RedrawWindow ();
}

void CMainFrame::OnViewHistorybar()
{
	ShowControlBar (&m_wndHistoryBar, !(m_wndHistoryBar.IsVisible ()), FALSE, TRUE);
	RecalcLayout();
}

void CMainFrame::OnUpdateViewHistorybar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (m_wndHistoryBar.IsVisible ());
	pCmdUI->Enable (!GetDockManager ()->IsPrintPreviewValid ());
}

void CMainFrame::OnViewInformationbar()
{
	ShowControlBar (&m_wndInfoBar, !(m_wndInfoBar.IsVisible ()), FALSE, TRUE);
	RecalcLayout();
}

void CMainFrame::OnUpdateViewInformationbar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (m_wndInfoBar.IsVisible ());
	pCmdUI->Enable (!GetDockManager ()->IsPrintPreviewValid ());
}


/*

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
CBCGPFrameWnd::OnSize(nType, cx, cy);
RecalcLayout();
}

#define WIDTHBYTES(bits) (((bits)+31)/32*4)
void CMainFrame::OnCopy()
{
	if (this->GetSafeHwnd()==NULL) return;
	
	int W = girlStatistics("filmXres");
	int H = girlStatistics("filmYres");

	if (!::OpenClipboard(this->GetSafeHwnd()))
	{                                     
#ifdef _DEBUG
		TRACE("Cannot open clipboard.\n");
#endif
		return;
	}
	
	DWORD dwSize = 0;

	// 4byte 배수의 width가 맞는 경우
	if (W%4 == 0)
		dwSize = (H * (W*3));	// height * (width & biBitCount/8)
	else
	{
		dwSize = (H * (W*3)) + (((W%4) & H) + 4 - (W%4));
	}


	CWindowDC dc(this);
	CBitmap* pbmOld = NULL;
	CDC dcMem;
	VERIFY(dcMem.CreateCompatibleDC(&dc));

	CBitmap bm;
	VERIFY(bm.CreateCompatibleBitmap(&dc, W, H));
	ASSERT(bm.m_hObject != NULL);
	pbmOld = dcMem.SelectObject(&bm);

	dcMem.PatBlt(0, 0, W, H, WHITENESS);
	//VERIFY(dcMem.BitBlt(0, 0, W, H, girlFramebuffer(), 0, 0, SRCCOPY));    

	bm.SetBitmapBits (dwSize, girlFramebuffer ());

	HGDIOBJ hBM=bm.Detach();
	VERIFY(::EmptyClipboard());
	VERIFY(::SetClipboardData(CF_BITMAP, hBM));
	VERIFY(::CloseClipboard());

	dcMem.SelectObject(pbmOld);
	dcMem.DeleteDC();
}
*/
void CMainFrame::OnFileOption()
{
	COptionPropertySheet propSheet (CBCGPPropertySheet::PropSheetLook_Tabs);
	propSheet.DoModal();
}
