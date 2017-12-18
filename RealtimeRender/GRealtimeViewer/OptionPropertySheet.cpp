//
// OptionPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionPropertySheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionPropertySheet

IMPLEMENT_DYNAMIC(COptionPropertySheet, CBCGPPropertySheet)

COptionPropertySheet::COptionPropertySheet(CBCGPPropertySheet::PropSheetLook look,
								   UINT uiIconsResID, int cxIcon)
	 : CBCGPPropertySheet(_T("Render options"), NULL)
{
	SetLook (look);

	if (uiIconsResID != 0)
	{
		SetIconsList (uiIconsResID, cxIcon);
	}

	if (look == PropSheetLook_Tree)
	{
		CBCGPPropSheetCategory* pCategory1 = AddTreeCategory (_T("Category 1"), 0, 1);
		AddPageToTree (pCategory1, &m_PageToneMapping, -1, 2);
		AddPageToTree (pCategory1, &m_PageSystem, -1, 2);

		CBCGPPropSheetCategory* pCategory2 = AddTreeCategory (_T("Category 2"), 0, 1);
	}
	else
	{
		AddPage(&m_PageToneMapping);
		m_PageToneMapping.SetOwner (this);

		AddPage(&m_PageSystem);
		m_PageSystem.SetOwner (this);
	}
}

COptionPropertySheet::~COptionPropertySheet()
{
}


BEGIN_MESSAGE_MAP(COptionPropertySheet, CBCGPPropertySheet)
	//{{AFX_MSG_MAP(COptionPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COptionPropertySheet message handlers


void COptionPropertySheet::OnDrawPageHeader (CDC* pDC, int nPage, CRect rectHeader)
{
	rectHeader.top += 2;
	rectHeader.right -= 2;
	rectHeader.bottom -= 2;

	if (IsVisualManagerStyle ())
	{
		pDC->FillRect (rectHeader, &globalData.brBarFace);
		pDC->Draw3dRect (rectHeader, globalData.clrBarShadow, globalData.clrBarShadow);
		pDC->SetTextColor (globalData.clrBarText);
	}
	else
	{
		pDC->FillRect (rectHeader, &globalData.brBtnFace);
		pDC->Draw3dRect (rectHeader, globalData.clrBtnShadow, globalData.clrBtnShadow);
		pDC->SetTextColor (globalData.clrBtnText);
	}

	CBCGPDrawManager dm (*pDC);
	dm.DrawShadow (rectHeader, 2);	

	CString strText;
	strText.Format (_T("Page %d description..."), nPage + 1);

	CRect rectText = rectHeader;
	rectText.DeflateRect (10, 0);

	CFont* pOldFont = pDC->SelectObject (&globalData.fontBold);
	pDC->SetBkMode (TRANSPARENT);

	pDC->DrawText (strText, rectText, DT_SINGLELINE | DT_VCENTER);

	pDC->SelectObject (pOldFont);
}
