#pragma once
#include "afxwin.h"

#include "bcgpedit.h"
#include "bcgpbutton.h"
#include "bcgpprogressctrl.h"

enum GRealtimeCtlMode
{
	CTL_PAUSE,
	CTL_RESUME,
	CTL_STOP
};

// CInfoBar dialog

class CImgButton: public CBCGPButton
{
	DECLARE_DYNCREATE(CImgButton)

public:
	CImgButton ()
	{
	}
	CImgButton (const CSize& sizeImage, const CString& strImage);

	void ImgChange (const CString& strImage);

protected:
	virtual void OnFillBackground (CDC* pDC, const CRect& rectClient)
	{
		if (!m_Image.IsValid ())
		{
			CBCGPButton::OnFillBackground (pDC, rectClient);
		}
	}
	virtual void OnDrawBorder (CDC* pDC, CRect& rectClient, UINT uiState)
	{
		if (!m_Image.IsValid ())
		{
			CBCGPButton::OnDrawBorder (pDC, rectClient, uiState);
		}
	}
	virtual void OnDrawFocusRect (CDC* pDC, const CRect& rectClient)
	{
		if (!m_Image.IsValid ())
		{
			CBCGPButton::OnDrawFocusRect (pDC, rectClient);
		}
	}
	virtual void OnDraw (CDC* pDC, const CRect& rect, UINT uiState);

private:
	CBCGPToolBarImages	m_Image;
	CSize m_sizeImage;
};


class CInfoBar : public CBCGPDialogBar
{
	DECLARE_DYNAMIC(CInfoBar)

public:
	CInfoBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInfoBar();

// Dialog Data
	enum { IDD = IDD_DIALOG_INFO };

public:
	void SetFPS( float fps );
	void SetData (CString sTime, int samplesSec, int samplesTotSec, int haltspp, double samplesPx);
	void SetProgressDone ();
	UINT GetNumOfThread(void);
	LRESULT OnTHREAD_NUM_CHANGE (WPARAM wParam, LPARAM lParam);
	void OnClickedBtnPause();
	void OnClickedBtnStop();
	BOOL IsPaused ();
	void EnablePauseButton (BOOL bEnable);
	void EnableStopButton (BOOL bEnable);
	void Init();
private:
	UINT m_nThread;
	BOOL m_bThreadUpdate;
	CBrush m_brEdit;
	CImgButton m_btnPause;
	CImgButton m_btnStop;
	CSpinButtonCtrl m_ctrlSpinThread;
	CBCGPProgressCtrl m_ctrlProgress;
	BOOL m_bPause;
	UINT m_nMaxThread;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDeltaposSpinThread(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
