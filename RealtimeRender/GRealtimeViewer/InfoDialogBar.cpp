// InfoDialogBar.cpp : implementation file
//

#include "stdafx.h"
#include "GRealtimeView.h"
#include "InfoDialogBar.h"

#include <conio.h>

// CInfoDialogBar dialog

IMPLEMENT_DYNAMIC(CInfoDialogBar, CBCGPDialogBar)

CInfoDialogBar::CInfoDialogBar(CWnd* pParent /*=NULL*/)
: m_bProcess(FALSE)
{
	SYSTEM_INFO info={0};
	GetSystemInfo(&info);
	m_nThread = info.dwNumberOfProcessors;
_cprintf ("%d \n", m_nThread);
	//m_nThread = 1;
	m_bProcess = FALSE;

	m_bThreadUpdate = FALSE;
}

CInfoDialogBar::~CInfoDialogBar()
{
}

void CInfoDialogBar::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialogBar::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProgress);
	DDX_Control(pDX, IDC_SPIN_THREAD, m_ctrlSpinThread);
	DDX_Text(pDX, IDC_EDIT_THREAD, m_nThread);
	DDV_MinMaxUInt(pDX, m_nThread, 0, 99);
	DDX_Control(pDX, IDC_BUTTON_PAUSE, m_btnPause);
}


BEGIN_MESSAGE_MAP(CInfoDialogBar, CBCGPDialogBar)
	ON_WM_CREATE()
	ON_EN_CHANGE(IDC_EDIT_THREAD, &CInfoDialogBar::OnEnChangeEditThread)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_THREAD, &CInfoDialogBar::OnDeltaposSpinThread)
	ON_REGISTERED_MESSAGE(BMSG_THREAD_NUM_CHANGE_OUT, OnTHREAD_NUM_CHANGE)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, OnBnClickedButtonPause)
END_MESSAGE_MAP()


// CInfoDialogBar message handlers
int CInfoDialogBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_ctrlProgress.SetRange (0, 100);
	m_ctrlSpinThread.SetRange (1, 99);

//	m_nThread = 1;

// 	LONG style = ::GetWindowLong (this->m_hWnd, GWL_STYLE);
// 	style &= ~ WS_CAPTION;
// 	style &= ~ WS_THICKFRAME;
// 	::SetWindowLong (this->m_hWnd, GWL_STYLE, style);

	return 0;
}

void CInfoDialogBar::SetData (CString sTime, int samplesSec, int samplesTotSec, int haltspp, double samplesPx)
{
	SetDlgItemText (IDC_EDIT_TIME_PASSED, sTime);
	SetDlgItemInt (IDC_EDIT_SAMPLES_PER_SEC, samplesSec);
	SetDlgItemInt (IDC_EDIT_TOTAL_SPS, samplesTotSec);
	SetDlgItemInt (IDC_EDIT_MAXIMUM_SPP, haltspp);
	SetDlgItemInt (IDC_EDIT_AVERAGE_SPP, samplesPx);

	if (haltspp != 0)
	{
		int nPercent = (int)((samplesPx/haltspp) * 100);
		m_ctrlProgress.SetPos (nPercent);

		CString sPercent;
		sPercent.Format ("%d %%", nPercent);
		SetDlgItemText (IDC_STATIC_PROGRESS, sPercent);
	}
	else
		SetDlgItemText (IDC_STATIC_PROGRESS, "unlimit");
	
	UpdateData (FALSE);
}

void CInfoDialogBar::OnEnChangeEditThread()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBCGPDialogBar::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	if (m_bThreadUpdate)
	{
		UpdateData (TRUE);
		::PostMessage (HWND_BROADCAST, BMSG_THREAD_NUM_CHANGE_IN, m_nThread, 0L);
		_cprintf ("%d sent \n", m_nThread);
	}

	m_bThreadUpdate = FALSE;
}

void CInfoDialogBar::OnDeltaposSpinThread(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	m_bThreadUpdate = TRUE;

	*pResult = 0;
}

LRESULT CInfoDialogBar::OnTHREAD_NUM_CHANGE (WPARAM wParam, LPARAM lParam)
{
	m_nThread = (UINT)wParam;
	UpdateData (FALSE);
	_cprintf ("info===> %d \n", m_nThread);
	m_bThreadUpdate = FALSE;

	return 0L;
}

void CInfoDialogBar::OnBnClickedButtonPause()
{
	_cprintf ("pressed\n");
}

UINT CInfoDialogBar::GetNumOfThread()
{
	UpdateData (TRUE);

	return m_nThread;
}