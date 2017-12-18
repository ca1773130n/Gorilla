//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a sample for BCGControlBar Library Professional Edition
// Copyright (C) 1998-2008 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// PreviewBar.cpp: implementation of the CResourceViewBar class.
//

#include "stdafx.h"
#include "GRealtimeViewer.h"
#include "MainFrm.h"
#include "PreviewBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistoryBar

CPreviewBar::CPreviewBar()
{
}

CPreviewBar::~CPreviewBar()
{
}

BEGIN_MESSAGE_MAP(CPreviewBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CPreviewBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CPreviewBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

// 	m_pwndStatic = new CBCGPStatic;
// 	if (!m_pwndStatic->Create (_T("image"), WS_VISIBLE|WS_CHILD, CRect(10,10,100,100), this, 3333))
// 		return -1;
// 	m_pwndStatic->ShowWindow (SW_SHOW);

//	m_Font.CreateStockObject (DEFAULT_GUI_FONT);

// 	CRect rectClient (0, 0, lpCreateStruct->cx, lpCreateStruct->cy);
// 
//  Create output pane:
// 	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;
// 
// 	if (!m_wndOutput.Create (dwStyle, rectClient, this, 1))
// 	{
// 		TRACE0("Failed to create output window\n");
// 		return -1;      // fail to create
// 	}
// 
// 	m_wndOutput.SetFont (&m_Font);
// 
// 	// Fill view context (dummy code, don't seek here something magic :-)):
// 	FillDebugWindow ();
	return 0;
}

void CPreviewBar::OnSize(UINT nType, int cx, int cy) 
{
//	GetClientRect (&m_rectClient);

// 	UINT nMargin = 10;
// 	m_pwndStatic->SetWindowPos (&wndTop, nMargin, nMargin, m_rectClient.right-nMargin*2, m_rectClient.bottom-nMargin*2, SWP_SHOWWINDOW);

	CBCGPDockingControlBar::OnSize(nType, cx, cy);
}

// void CPreviewBar::AdjusrHorzScroll (CListBox& wndListBox)
// {
// 	CClientDC dc (this);
// 	CFont* pOldFont = dc.SelectObject (&m_Font);
// 
// 	int cxExtentMax = 0;
// 
// 	for (int i = 0; i < wndListBox.GetCount (); i ++)
// 	{
// 		CString strItem;
// 		wndListBox.GetText (i, strItem);
// 
// 		cxExtentMax = max (cxExtentMax, dc.GetTextExtent (strItem).cx);
// 	}
// 	
// 	wndListBox.SetHorizontalExtent (cxExtentMax);
// 	dc.SelectObject (pOldFont);
//}

// void CPreviewBar::FillDebugWindow ()
// {
// 	m_wndOutput.AddString (_T("Preview preview"));
// 	m_wndOutput.AddString (_T("preview"));
//}

void CPreviewBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectClient;
	GetClientRect (rectClient);
	//	ScreenToClient (rectClient);

	rectClient.InflateRect (1, 1);

	dc.FillSolidRect(rectClient, RGB(31,31,31));
}

// void CPreviewBar::OnSetFocus(CWnd* pOldWnd) 
// {
// 	CBCGPDockingControlBar::OnSetFocus(pOldWnd);
// 	m_wndOutput.SetFocus ();
//}
