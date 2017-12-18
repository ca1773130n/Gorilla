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
// PreviewBar.h: interface for the CPreviewBar class.
//


#if !defined(AFX_PREVIEWBAR_H__F9C28949_2C41_47AD_9BAB_E2516CEEC6E0__INCLUDED_)
#define AFX_PREVIEWBAR_H__F9C28949_2C41_47AD_9BAB_E2516CEEC6E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPreviewBar : public CBCGPDockingControlBar  
{
// Construction
public:
	CPreviewBar();

// Attributes
protected:

//	CFont			m_Font;
	CBCGPStatic*	m_pwndStatic;
	CRect			m_rectClient;

protected:
// 	void FillDebugWindow ();
// 	void FillBuildWindow ();
//	void AdjusrHorzScroll (CListBox& wndListBox);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreviewBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPreviewBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPreviewBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
//	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_PREVIEWBAR_H__F9C28949_2C41_47AD_9BAB_E2516CEEC6E0__INCLUDED_)
