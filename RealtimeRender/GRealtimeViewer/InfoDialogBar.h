#pragma once
#include "bcgpedit.h"
#include "bcgpbutton.h"
#include "afxwin.h"
#include "bcgpprogressctrl.h"


// CInfoDialogBar dialog

class CInfoDialogBar : public CBCGPDialogBar
{
	DECLARE_DYNAMIC(CInfoDialogBar)

public:
	CInfoDialogBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInfoDialogBar();

// Dialog Data
	enum { IDD = IDD_DIALOG_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CBCGPSpinButtonCtrl m_ctrlSpinThread;
	UINT m_nThread;
	BOOL m_bThreadUpdate;
	CBCGPButton m_btnPause;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
private:
	BOOL m_bProcess;

public:
	void SetData (CString sTime, int samplesSec, int samplesTotSec, int haltspp, double samplesPx);
	LRESULT OnTHREAD_NUM_CHANGE (WPARAM wParam, LPARAM lParam);

public:
	CBCGPProgressCtrl m_ctrlProgress;
	afx_msg void OnEnChangeEditThread();
	afx_msg void OnDeltaposSpinThread(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonPause();
	UINT GetNumOfThread(void);
};
