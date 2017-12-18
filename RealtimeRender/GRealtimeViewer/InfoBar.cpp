// InfoBar.cpp : implementation file
//

#include "stdafx.h"
#include "GRealtimeViewer.h"
#include "InfoBar.h"

#include <conio.h>
#include "MainFrm.h"

// CInfoBar dialog

IMPLEMENT_DYNCREATE(CImgButton, CBCGPButton)

CImgButton::CImgButton (const CSize& sizeImage, const CString& strImage)
{
	if (!strImage.IsEmpty ())
	{
		m_sizeImage = sizeImage;
		m_Image.SetImageSize (m_sizeImage);
		m_Image.LoadStr (strImage);

		m_bTransparent = TRUE;
	}
}

void CImgButton::ImgChange (const CString& strImage)
{
	m_Image.SetImageSize (m_sizeImage);
	m_Image.LoadStr (strImage);

	m_bTransparent = TRUE;

	//Invalidate ();
}

void CImgButton::OnDraw (CDC* pDC, const CRect& rect, UINT uiState)
{
	if (!m_Image.IsValid ())
	{
		CBCGPButton::OnDraw (pDC, rect, uiState);
		return;
	}

	BOOL bPressed     = IsPressed ();
	BOOL bHighlighted = IsHighlighted ();

	int index = 0;
	if (bPressed)
	{
		if (bHighlighted)
		{
			index = 2;
		}
	}
	else if (bHighlighted)
	{
		index = 1;
	}

	m_Image.DrawEx (pDC, rect, index, 
		CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
}


IMPLEMENT_DYNAMIC(CInfoBar, CBCGPDialogBar)

CInfoBar::CInfoBar(CWnd* pParent /*=NULL*/)
	: CBCGPDialogBar  ()
	, m_btnPause (CSize (18, 18), _T("IDR_BTN_PAUSE"))
	, m_btnStop (CSize (18, 18), _T("IDR_BTN_STOP"))
{
	SYSTEM_INFO info={0};
	GetSystemInfo(&info);
	m_nMaxThread = info.dwNumberOfProcessors;
	//m_nThread = 1;
	m_nThread = m_nMaxThread;

	m_bThreadUpdate = FALSE;

	m_bPause = FALSE;
}

CInfoBar::~CInfoBar()
{
}

void CInfoBar::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialogBar::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_THREAD, m_nThread);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProgress);
	DDX_Control(pDX, IDC_SPIN_THREAD, m_ctrlSpinThread);
	DDX_Control(pDX, IDC_BTN_PAUSE, m_btnPause);
	DDX_Control(pDX, IDC_BTN_STOP, m_btnStop);
}


BEGIN_MESSAGE_MAP(CInfoBar, CBCGPDialogBar)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_PAUSE, &CInfoBar::OnClickedBtnPause)
	ON_BN_CLICKED(IDC_BTN_STOP, &CInfoBar::OnClickedBtnStop)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_THREAD, &CInfoBar::OnDeltaposSpinThread)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CInfoBar message handlers
int CInfoBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	_cprintf ("Max thread: %d \n", m_nMaxThread);

	m_brEdit.CreateSolidBrush(RGB (83,83,83));

	m_btnStop.m_bTransparent = TRUE;
	m_btnPause.m_bTransparent = TRUE;

	return 0;
}

void CInfoBar::Init(VOID)
{
	m_ctrlProgress.SetRange (1, 100);
	m_ctrlSpinThread.SetRange (1, m_nMaxThread);
	m_btnPause.ImgChange(_T("IDR_BTN_PAUSE"));
	m_btnStop.ImgChange(_T("IDR_BTN_STOP"));
	//m_btnPause.SizeToContent();
	//m_btnStop.SizeToContent();
}

void CInfoBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect r;
	GetClientRect(&r);

	dc.FillSolidRect(r, RGB(81,81,91));
}

HBRUSH CInfoBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CBCGPDialogBar::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID () == IDC_EDIT_TIME_PASSED ||
		pWnd->GetDlgCtrlID () == IDC_EDIT_SAMPLES_PER_SEC ||
		pWnd->GetDlgCtrlID () == IDC_EDIT_TOTAL_SPS ||
		pWnd->GetDlgCtrlID () == IDC_EDIT_MAXIMUM_SPP ||
		pWnd->GetDlgCtrlID () == IDC_EDIT_AVERAGE_SPP ||
		pWnd->GetDlgCtrlID () == IDC_EDIT_PROGRESS ||
		pWnd->GetDlgCtrlID () == IDC_EDIT_THREAD)
	{
		pDC->SetBkColor (RGB(83,83,83));
		pDC->SetTextColor (RGB (204,146,64));
		//pDC->SetBkMode(TRANSPARENT);
		//pDC->SetTextAlign (TA_CENTER|TA_BASELINE);
		hbr = (HBRUSH)m_brEdit;
	}
	else if (pWnd->GetDlgCtrlID () == IDC_STATIC)
	{
		pDC->SetBkColor (RGB(83,83,83));
	}

	return hbr;
}

void CInfoBar::SetProgressDone ()
{
	m_ctrlProgress.SetPos (100);
}

void CInfoBar::SetFPS( float fps )
{
	SetDlgItemInt(IDC_EDIT_SAMPLES_PER_SEC, int(fps));
}
void CInfoBar::SetData (CString sTime, int samplesSec, int samplesTotSec, int haltspp, double samplesPx)
{
	SetDlgItemText (IDC_EDIT_TIME_PASSED, sTime);
	SetDlgItemInt (IDC_EDIT_SAMPLES_PER_SEC, samplesSec);
	SetDlgItemInt (IDC_EDIT_TOTAL_SPS, samplesTotSec);
	SetDlgItemInt (IDC_EDIT_MAXIMUM_SPP, haltspp);
	SetDlgItemInt (IDC_EDIT_AVERAGE_SPP, (int)samplesPx);

	if (haltspp != 0)
	{
		int nPercent = (int)((samplesPx/haltspp) * 100);

		if (nPercent >= 99)
			nPercent = 100;

		m_ctrlProgress.SetPos (nPercent);

		CString sPercent;
		sPercent.Format (L"%d %%", nPercent);
		SetDlgItemText (IDC_EDIT_PROGRESS, sPercent);
	}
	else
		SetDlgItemText (IDC_EDIT_PROGRESS, L"Unlimited");

	UpdateData (FALSE);
}

void CInfoBar::OnDeltaposSpinThread(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData (TRUE);

	(pNMUpDown->iDelta < 0) ? m_nThread++ : m_nThread--;

	if (m_nThread >= m_nMaxThread) m_nThread = m_nMaxThread;
	if (m_nThread <= 1) m_nThread = 1;
	
	_cprintf ("info: %d\n", m_nThread);
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd ();
	pFrame->SetRenderThreads(m_nThread);

	UpdateData (FALSE);

	*pResult = 0;
}

UINT CInfoBar::GetNumOfThread()
{
	UpdateData (TRUE);

	return m_nThread;
}

void CInfoBar::OnClickedBtnPause()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd ();
	if (pFrame->m_guiRenderState == WAITING)
		return;

	if (m_bPause ^= TRUE)
	{
		m_btnPause.ImgChange(_T("IDR_BTN_RESUME"));
		m_btnPause.SetWindowText (_T("Resume"));

		pFrame->doAction (CTL_PAUSE); // pause


	}
	else
	{
		m_btnPause.ImgChange(_T("IDR_BTN_PAUSE"));
		m_btnPause.SetWindowText (_T("Pause"));

		pFrame->doAction (CTL_RESUME); // resume
	}
}

void CInfoBar::OnClickedBtnStop()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd ();
	if (pFrame->m_guiRenderState == WAITING)
		return;

	pFrame->doAction (CTL_STOP); // stop
	m_btnStop.ImgChange(_T("IDR_BTN_STOP"));
}

BOOL CInfoBar::IsPaused ()
{
	return m_bPause;
}

void CInfoBar::EnablePauseButton (BOOL bEnable)
{
	m_btnPause.EnableWindow (bEnable);
}

void CInfoBar::EnableStopButton (BOOL bEnable)
{
	m_btnStop.EnableWindow (bEnable);
}
void CInfoBar::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDialogBar::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}
