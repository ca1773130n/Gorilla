#pragma once

#include "resource.h"

// CLoadingDlg dialog

class CLoadingDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoadingDlg)

public:
	CLoadingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoadingDlg();

	UINT m_res;
	CBitmap m_bmp;
	BOOL m_bAppear;

	int StartAnimation(void);
	int StopAnimation(void);
	LRESULT OnCloseForced (WPARAM wParam, LPARAM lParam);

// Dialog Data
	enum { IDD = IDD_DIALOG_LOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
};
