// GRealtimeViewer.h : main header file for the GRealtimeViewer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "GRenderer.h"
#include "GServer.h"

// namespace girl
// {

// CGRealtimeViewerApp:
// See GRealtimeViewer.cpp for the implementation of this class
//

class CGRealtimeViewerApp : public CWinApp, public CBCGPWorkspace
{
public:
	CGRealtimeViewerApp();
	bool ProcessCommandLine();

	// Override from CBCGPWorkspace
	virtual void PreLoadState ();

	CGRenderer&		GetRenderer( VOID ) { return *m_pRenderer; }

// Overrides
public:
	virtual int Run();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	
// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
	CGRenderer*		m_pRenderer;
	CGServer*		m_pServer;
};

extern CGRealtimeViewerApp theApp;

/*}//namespace girl*/