// Loading.cpp : implementation file
//

#include "stdafx.h"
#include "GRealtimeViewer.h"
#include "Loading.h"


// CLoadingDlg dialog

IMPLEMENT_DYNAMIC(CLoadingDlg, CDialog)

CLoadingDlg::CLoadingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadingDlg::IDD, pParent)
{

}

CLoadingDlg::~CLoadingDlg()
{
}

void CLoadingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoadingDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_REGISTERED_MESSAGE (UM_CLOSE_WND, OnCloseForced)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CLoadingDlg message handlers

void CLoadingDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CRect r;
	GetClientRect (&r);

	CDC dcMem;
	dcMem.CreateCompatibleDC (&dc);
	dcMem.SelectObject(&m_bmp);

	dc.FillSolidRect (&r, RGB (127,127,127));

	dc.TransparentBlt (0, 0, 64, 64, &dcMem, 0, 0, 64, 64, RGB (127,127,127));

	if (m_bAppear)
	{
		CRect rTxt(0,0,64,64);
		dc.SetBkMode (TRANSPARENT);
		dc.DrawText (L"load", &rTxt, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
	}
}

int CLoadingDlg::StartAnimation(void)
{
	m_bAppear = FALSE;
	m_res = IDB_BITMAP1;
	SetTimer (1, 100, NULL);
	SetTimer (2, 800, NULL);

	return 0;
}

int CLoadingDlg::StopAnimation(void)
{
	KillTimer (1);
	KillTimer (2);

	ShowWindow (SW_HIDE);
	OnClose ();
	
	return 0;
}

void CLoadingDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		if (m_res < IDB_BITMAP1)
			m_res = IDB_BITMAP12;

		//		_cprintf ("%d \n", m_res);

		m_bmp.LoadBitmap (m_res--);
		CRect r(0,0,64,64);
		InvalidateRect (&r);
	}
	else if (nIDEvent == 2)
	{
		m_bAppear ^= TRUE;
	}

	CDialog::OnTimer(nIDEvent);
}

void CLoadingDlg::OnClose()
{
	CDialog::OnClose();
}


LRESULT CLoadingDlg::OnCloseForced (WPARAM wParam, LPARAM lParam)
{
	StopAnimation();

	return 0L;
}