// MainFrm.h : interface of the CMainFrame class
//

#include "InfoBar.h"
#include "HistoryBar.h"

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <string>

#include "Loading.h"

#include "GRenderer.h"
#include "GServer.h"

#pragma once

// namespace girl
// {

#define ID_RENDERUPDATE	2000
#define ID_STATSUPDATE	2001
#define ID_LOADUPDATE	2002
#define ID_SPLASHUPDATE	2003
#define ID_TIMEOUT		2004

class GRealtimeError {
public:
	GRealtimeError(int code, int severity, const char *msg): m_code(code), m_severity(severity), m_msg(msg) {}

	int GetCode() { return m_code; }
	int GetSeverity() { return m_severity; }
	const std::string& GetMessage() { return m_msg; }

protected:
	int m_code;
	int m_severity;
	std::string m_msg;
};

/*** GRealtimeGui ***/
enum GRealtimeGuiRenderState
{
	WAITING,
	RENDERING,
	STOPPING,
	STOPPED,
	PAUSED,
	FINISHED
};
enum GRealtimeGuiWindowState
{
	SHOWN,
	HIDDEN
};

class CMainFrame : public CBCGPFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	BOOL m_bOpenGL;
	BOOL m_bCopy2Console;
	int m_numThreads;

	CEvent m_eventEnd_ToneMapped;

	CBCGPRibbonEdit* m_pEditThread;
	CBCGPProgressDlg* m_pProgressDlg;
	UINT_PTR m_timerSplash;
	BOOL m_bSceneOpened;

	CLoadingDlg* m_pLoadingDlg;

// Operations
public:
	
// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	CBCGPRibbonStatusBar	m_wndStatusBar;
	CBCGPRibbonBar			m_wndRibbonBar;
	CBCGPRibbonMainButton	m_MainButton;
	CBCGPToolBarImages		m_PanelImages;
	CBCGPMenuBar			m_wndMenuBar;
	CBCGPToolBar			m_wndToolBar;
	
	CInfoBar				m_wndInfoBar;
	CHistoryBar				m_wndHistoryBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAppLook(UINT id);
	afx_msg void OnUpdateAppLook(CCmdUI* pCmdUI);
	afx_msg LRESULT OnRibbonCustomize (WPARAM wp, LPARAM lp);
	afx_msg void OnToolsOptions();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

	BOOL CreateRibbonBar ();
	void ShowOptions (int nPage);

	UINT	m_nAppLook;
public:
	LRESULT OnTHREAD_NUM_CHANGE (WPARAM wParam, LPARAM lParam);
	long OnUm_UpdateThread (WPARAM wParam, LPARAM lParam);
	long OnUm_ResumeAction (WPARAM wParam, LPARAM lParam);
	/*long OnUm_WriteImage (WPARAM wParam, LPARAM lParam);*/
	LRESULT OnUm_LogMsg (WPARAM wParam, LPARAM lParam);

	void SetRenderThreads(int nNumOfThread);
	void RenderSceneFile(LPCTSTR filename);
	void ChangeRenderState(GRealtimeGuiRenderState state);
	void doAction(GRealtimeCtlMode ctlMode);
	void ShowSplash (BOOL bIsCreate, CString sCaption, CString sMsg1, CString sMsg2);
	void SetStr2Status (CString sMsg);

	// Parsing and rendering threads
	void UpdateThread();
	void EngineThread();

	void UpdateStatistics();

	boost::thread *m_engineThread, *m_updateThread;
	double m_samplesSec;
	GRealtimeGuiRenderState m_guiRenderState;
	GRealtimeGuiWindowState m_guiWindowState;
	CString m_sFilePath;

	UINT_PTR m_renderTimer;
	UINT_PTR m_statsTimer;
	UINT_PTR m_loadTimer;
	UINT_PTR m_timeoutTimer;

	void SetOpenFile(LPCTSTR lpszFilePath, int nTime = 0);
	afx_msg void OnFileOpenscene();
	
	afx_msg void OnViewHistory();
	afx_msg void OnUpdateViewHistory(CCmdUI *pCmdUI);
	afx_msg void OnClose();
		
	afx_msg void OnViewHistorybar();
	afx_msg void OnUpdateViewHistorybar(CCmdUI *pCmdUI);
	afx_msg void OnViewInformationbar();
	afx_msg void OnUpdateViewInformationbar(CCmdUI *pCmdUI);
	afx_msg void OnFileOption();
	afx_msg void OnBnClickedBtnPause();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnUpdateFileOpenscene(CCmdUI *pCmdUI);
	
	/*
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCopy();
	*/
	

public:
	CHistoryBar&	GetHistoryBar() { return m_wndHistoryBar; }
	void			DoPostParsing();
	CGRenderer*		GetRenderer() { return &m_Renderer; }
	CGServer*		GetServer() { return &m_Server; }

private:
	CGRenderer		m_Renderer;
	CGServer		m_Server;
};

void GRealtimeGuiErrorHandler (int code, int serverity, const char* msg);

//} //namespace girl
