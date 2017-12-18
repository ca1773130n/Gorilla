
 
#ifndef __OPTIONPROPERTYSHEET_H__
#define __OPTIONPROPERTYSHEET_H__

#include "OptionPages.h"

/////////////////////////////////////////////////////////////////////////////
// COptionPropertySheet

class COptionPropertySheet : public CBCGPPropertySheet
{
	DECLARE_DYNAMIC(COptionPropertySheet)

// Construction
public:
	COptionPropertySheet(CBCGPPropertySheet::PropSheetLook look,
					UINT uiIconsResID = 0, int cxIcon = 0);

// Attributes
public:
	CPageToneMapping m_PageToneMapping;
	CPageSystem m_PageSystem;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionPropertySheet)
	//}}AFX_VIRTUAL

	virtual void OnDrawPageHeader (CDC* pDC, int nPage, CRect rectHeader);

// Implementation
public:
	virtual ~COptionPropertySheet();

// Generated message map functions
protected:
	//{{AFX_MSG(COptionPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __OPTIONPROPERTYSHEET_H__
