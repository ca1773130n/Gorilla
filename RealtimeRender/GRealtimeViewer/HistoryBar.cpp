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
// HistoryBar.cpp: implementation of the CResourceViewBar class.
//

#include "stdafx.h"
#include "GRealtimeViewer.h"
#include "MainFrm.h"
#include "HistoryBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistoryBar

CHistoryBar::CHistoryBar()
{
	m_dwCnt = 0;
}

CHistoryBar::~CHistoryBar()
{
}

BEGIN_MESSAGE_MAP(CHistoryBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CHistoryBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CHistoryBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	//m_Font.CreateStockObject (DEFAULT_GUI_FONT);

	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	// Create output pane:
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL | LVS_REPORT | LVS_SHOWSELALWAYS;

	m_wndHistory.Create (dwStyle, rectDummy, this, ID_LIST_HISTORY);
	m_wndHistory.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	m_wndHistory.InsertColumn (0, _T("log"), LVCFMT_LEFT, 1600);
		
	//m_wndHistory.SetFont (&m_Font);

	m_wndHistory.SetBkColor(RGB (71,71,71));
	//m_wndHistory.SetTextColor (RGB (204,146,64));
	m_wndHistory.SetTextColor (RGB (127,127,127));
	m_wndHistory.SetTextBkColor(RGB (71,71,71));
	


	LOGFONT lf;
	globalData.fontRegular.GetLogFont (&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	globalData.GetNonClientMetrics (info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_Font.CreateFontIndirect (&lf);
	m_wndHistory.SetFont (&m_Font);


	return 0;
}

void CHistoryBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	if (CanAdjustLayout ())
	{
		CRect rc;
		GetClientRect(rc);

		m_wndHistory.SetWindowPos(NULL,
			rc.left + 1, rc.top + 1,
			rc.Width() - 2, rc.Height () - 2,
			SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CHistoryBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndHistory.GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	rectTree.InflateRect (1, 1);
	dc.Draw3dRect (rectTree, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));
}

void CHistoryBar::OnSetFocus(CWnd* pOldWnd) 
{
	CBCGPDockingControlBar::OnSetFocus(pOldWnd);
	m_wndHistory.SetFocus ();
}

void CHistoryBar::AddLog (CString sMsg)
{
	//m_wndHistory.InsertItem (m_dwCnt++, sMsg);
	m_wndHistory.InsertItem (0, sMsg);
//	m_wndHistory.SetCurSel (m_wndHistory.GetItemCount() - 1 )
//  	m_wndHistory.SetSelectionMark(0);
//  	m_wndHistory.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}