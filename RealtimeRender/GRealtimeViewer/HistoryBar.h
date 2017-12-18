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
// HistoryBar.h: interface for the CHistoryBar class.
//


#if !defined(AFX_HISTORYBAR_H__F9C28949_2C41_47AD_9BAB_E2516CEEC6E0__INCLUDED_)
#define AFX_HISTORYBAR_H__F9C28949_2C41_47AD_9BAB_E2516CEEC6E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHistoryBar : public CBCGPDockingControlBar  
{
// Construction
public:
	CHistoryBar();

// Attributes
protected:
	CFont		m_Font;
	CBCGPListCtrl	m_wndHistory;
	DWORD m_dwCnt;

public:
	CBCGPListCtrl* GetListCtrl () { return &m_wndHistory; };
	void AddLog (CString sMsg);

protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistoryBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHistoryBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHistoryBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_HISTORYBAR_H__F9C28949_2C41_47AD_9BAB_E2516CEEC6E0__INCLUDED_)
