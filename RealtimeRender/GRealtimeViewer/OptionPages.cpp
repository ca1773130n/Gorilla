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
// MyPropertyPage1.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionPages.h"

#include "OptionPropertySheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CPageToneMapping, CBCGPPropertyPage)
IMPLEMENT_DYNCREATE(CPageSystem, CBCGPPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// CMyPropertyPage1 property page

CPageToneMapping::CPageToneMapping() : CBCGPPropertyPage(CPageToneMapping::IDD)
{
	//{{AFX_DATA_INIT(CMyPropertyPage1)
	m_fPrescale = 0.0f;
	m_fPostscale = 0.0f;
	m_fBurn = 0.0f;
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPageToneMapping::~CPageToneMapping()
{
}

void CPageToneMapping::DoDataExchange(CDataExchange* pDX)
{
	CBCGPPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyPropertyPage1)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_COMBO1, m_nCombo);
	DDX_Text(pDX, IDC_EDIT_PRESCALE, m_fPrescale);
	DDX_Text(pDX, IDC_EDIT_POSTSCALE, m_fPostscale);
	DDX_Text(pDX, IDC_EDIT_BURN, m_fBurn);
}


BEGIN_MESSAGE_MAP(CPageToneMapping, CBCGPPropertyPage)
	//{{AFX_MSG_MAP(CMyPropertyPage1)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CPageToneMapping::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CPageToneMapping::OnBnClickedButtonRefresh)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_PRESCALE, &CPageToneMapping::OnNMCustomdrawSliderPrescale)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_POSTSCALE, &CPageToneMapping::OnNMCustomdrawSliderPostscale)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BURN, &CPageToneMapping::OnNMCustomdrawSliderBurn)
END_MESSAGE_MAP()

int CPageToneMapping::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CPageToneMapping::OnInitDialog()
{
	CBCGPPropertyPage::OnInitDialog();

// 	m_sliderPrescale.SetRange (-80, 80);
// 	m_sliderPrescale.SetPos (10);
// 
// 	m_sliderPostscale.SetRange (-60, 60);
// 	m_sliderPostscale.SetPos (10);
// 
// 	m_sliderBurn.SetRange (0, 100);
// 	m_sliderBurn.SetPos (60);

		return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPageToneMapping::OnNMCustomdrawSliderPrescale(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
// 	m_fPrescale = (float) m_sliderPrescale.GetPos () / 10;
// 	UpdateData (FALSE);

	*pResult = 0;
}

void CPageToneMapping::OnNMCustomdrawSliderPostscale(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
// 	m_fPostscale = (float) m_sliderPostscale.GetPos () / 10;
// 	UpdateData (FALSE);

	*pResult = 0;
}

void CPageToneMapping::OnNMCustomdrawSliderBurn(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
// 	m_fBurn = (float) m_sliderBurn.GetPos () / 10;
// 	UpdateData (FALSE);

	*pResult = 0;
}

void CPageToneMapping::OnBnClickedButtonReset()
{
	// TODO: Add your control notification handler code here
}

void CPageToneMapping::OnBnClickedButtonRefresh()
{
	// TODO: Add your control notification handler code here
}

/////////////////////////////////////////////////////////////////////////////
// CMyPropertyPage2 property page

CPageSystem::CPageSystem() : CBCGPPropertyPage(CPageSystem::IDD)
{
	//{{AFX_DATA_INIT(CMyPropertyPage2)
	m_nDisplayInterval = 0;
	m_nWriteInterval = 0;
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPageSystem::~CPageSystem()
{
}

void CPageSystem::DoDataExchange(CDataExchange* pDX)
{
	CBCGPPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyPropertyPage2)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_DISPLAY_INTERVAL, m_nDisplayInterval);
	DDX_Text(pDX, IDC_EDIT_WRITE_INTERVAL, m_nWriteInterval);
	DDX_Control(pDX, IDC_CHECK_WRITE_FLM, m_chkWriteFML);
	DDX_Control(pDX, IDC_CHECK_TONEMAPPED_TGA, m_chkTonemappedTga);
	DDX_Control(pDX, IDC_CHECK_TONEMAPPED_EXR, m_chkTonemappedExr);
	DDX_Control(pDX, IDC_CHECK_UNTONEMAPPED_EXR, m_chkUntonemappedExr);
	DDX_Control(pDX, IDC_CHECK_TONEMAPPED_IGI, m_chkTonemappedIgi);
	DDX_Control(pDX, IDC_CHECK_UNTONEMAPPED_IGI, m_chkUntonemappedIgi);
}


BEGIN_MESSAGE_MAP(CPageSystem, CBCGPPropertyPage)
	//{{AFX_MSG_MAP(CMyPropertyPage2)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CPageSystem::OnBnClickedButtonApply)
END_MESSAGE_MAP()

void CPageSystem::OnBnClickedButtonApply()
{
	// TODO: Add your control notification handler code here
}
