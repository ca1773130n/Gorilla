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
// MyPropertyPage1.h : header file
//

#ifndef __OPTIONPROPERTYPAGE_H__
#define __OPTIONPROPERTYPAGE_H__

/////////////////////////////////////////////////////////////////////////////
// CMyPropertyPage1 dialog

class CPageToneMapping : public CBCGPPropertyPage
{
	DECLARE_DYNCREATE(CPageToneMapping)

// Construction
public:
	CPageToneMapping();
	~CPageToneMapping();

// Dialog Data
	//{{AFX_DATA(CMyPropertyPage1)
	enum { IDD = IDD_PAGE_TONEMAPPING };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMyPropertyPage1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMyPropertyPage1)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CFont m_Font;
	int m_nCombo;
	float m_fPrescale;
	float m_fPostscale;
	float m_fBurn;
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnNMCustomdrawSliderPrescale(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderPostscale(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderBurn(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
};


/////////////////////////////////////////////////////////////////////////////
// CMyPropertyPage2 dialog

class CPageSystem : public CBCGPPropertyPage
{
	DECLARE_DYNCREATE(CPageSystem)

// Construction
public:
	CPageSystem();
	~CPageSystem();

// Dialog Data
	//{{AFX_DATA(CMyPropertyPage2)
	enum { IDD = IDD_PAGE_SYSTEM };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMyPropertyPage2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMyPropertyPage2)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CFont m_Font;
	UINT m_nDisplayInterval;
	UINT m_nWriteInterval;
	CButton m_chkWriteFML;
	CButton m_chkTonemappedTga;
	CButton m_chkTonemappedExr;
	CButton m_chkUntonemappedExr;
	CButton m_chkTonemappedIgi;
	CButton m_chkUntonemappedIgi;
	afx_msg void OnBnClickedButtonApply();
};


#endif // __OPTIONPROPERTYPAGE_H__
