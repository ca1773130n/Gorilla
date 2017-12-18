// GRealtimeViewerView.cpp : implementation of the CGRealtimeViewerView class
//

#include "stdafx.h"
#include "GRealtimeViewer.h"

#include "GRealtimeViewerDoc.h"
#include "GRealtimeViewerView.h"


#include <boost/bind.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/thread.hpp>
#include <boost/cast.hpp>

#include <direct.h>

#include <GL/glu.h>
#include <GL/gl.h>

#include <cmath>

#include "girl.h"
#include "api.h"

#include <conio.h>

using namespace girl;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGRealtimeViewerView

IMPLEMENT_DYNCREATE(CGRealtimeViewerView, CView)

BEGIN_MESSAGE_MAP(CGRealtimeViewerView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
//	ON_WM_PAINT()
/*
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_POPUP_SELECTION, &CGRealtimeViewerView::OnPopupSelection)
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_POPUP_SELECTION, &CGRealtimeViewerView::OnUpdatePopupSelection)
	ON_COMMAND(ID_POPUP_RESTORE, &CGRealtimeViewerView::OnPopupRestore)
	ON_COMMAND(ID_POPUP_ZOOMFIT, &CGRealtimeViewerView::OnPopupZoomfit)
	ON_WM_MBUTTONUP()
	*/
END_MESSAGE_MAP()

CGRealtimeViewerDoc* CGRealtimeViewerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGRealtimeViewerDoc)));
	return (CGRealtimeViewerDoc*)m_pDocument;

}
// CGRealtimeViewerView construction/destruction

CGRealtimeViewerView::CGRealtimeViewerView()// : wxGLCanvas(parent, wxID_ANY, glAttribList), wxViewerBase(), m_glContext(this), m_textureW(textureW), m_textureH(textureH)
{
	m_textureW = 256;
	m_textureH = 256;

	m_firstDraw = true;
	m_offsetX = m_offsetY = 0;
	m_scale = 1.0f;
	m_scaleExp=0;
	m_scaleXo = m_scaleYo = 0;
	m_scaleXo2 = m_scaleYo2 = 0;
	m_lastX = m_lastY = 0;

	m_stipple = 0x00FF; // Stipple pattern - dashes

	m_selectionChanged = false;
	m_refreshMarchingAntsOnly = false;

	/*
 	SetMode (PANZOOM);

	HCURSOR hCursor;
	hCursor = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
	SetCursor(hCursor);
	*/

	m_bLDown = m_bMDown = false;
	m_bSelection = m_bRestore = m_bZoomFit = false;
}

CGRealtimeViewerView::~CGRealtimeViewerView()
{
	KillTimer (m_timerAnimation);
}

BOOL CGRealtimeViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CGRealtimeViewerView drawing

void CGRealtimeViewerView::OnDraw(CDC* pDC)
{
	CGRealtimeViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}


// CGRealtimeViewerView printing

void CGRealtimeViewerView::OnFilePrintPreview()
{
	BCGPPrintPreview (this);
}

BOOL CGRealtimeViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGRealtimeViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGRealtimeViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CGRealtimeViewerView diagnostics

#ifdef _DEBUG
void CGRealtimeViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CGRealtimeViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

// CGRealtimeViewerDoc* CGRealtimeViewerView::GetDocument() const // non-debug version is inline
// {
// 	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGRealtimeViewerDoc)));
// 	return (CGRealtimeViewerDoc*)m_pDocument;
// }
#endif //_DEBUG


// CGRealtimeViewerView message handlers
/*
void CGRealtimeViewerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
}

void CGRealtimeViewerView::OnTimer(UINT_PTR nIDEvent)
{
	if (m_rectSelected.IsRectEmpty() && m_rectHighlightSel.IsRectEmpty ()) return;

	m_stipple = ((m_stipple >> 15) | (m_stipple << 1)) & 0xFFFF;
	if (!m_selectionChanged) {
		m_refreshMarchingAntsOnly = true;
	}
	m_selectionChanged = false;

	UpdateWindow ();

	CView::OnTimer(nIDEvent);
}

void CGRealtimeViewerView::Reload(void)
{
	m_imageChanged = true;
	Invalidate ();
}

void CGRealtimeViewerView::Reset(void)
{
	m_firstDraw = true;
	UpdateWindow ();
}

void CGRealtimeViewerView::InverseTransformPoint(int x, int y, int &invX, int &invY)
{
	CRect r;
	GetClientRect (&r);

	invX = (x - m_offsetX - m_scaleXo2)/m_scale + m_scaleXo;
	invY = (r.Height () - y - m_offsetY - m_scaleYo2)/m_scale + m_scaleYo;
}

void CGRealtimeViewerView::DrawMarchingAnts(CRect rectSelection, float red, float green, float blue)
{

}

void CGRealtimeViewerView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	GetClientRect (m_rectClient);
}

int CGRealtimeViewerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CGRealtimeViewerView::SetMode (ViewerMode mode)
{
}

void CGRealtimeViewerView::SetZoom(CRect rectSelected)
{	
}

void CGRealtimeViewerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CView::OnLButtonDown(nFlags, point);
}

void CGRealtimeViewerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CView::OnLButtonUp(nFlags, point);
}

void CGRealtimeViewerView::OnMouseMove(UINT nFlags, CPoint point)
{
	CView::OnMouseMove(nFlags, point);
}

void CGRealtimeViewerView::OnMButtonDown(UINT nFlags, CPoint point)
{
	CView::OnMButtonDown(nFlags, point);
}

void CGRealtimeViewerView::OnMButtonUp(UINT nFlags, CPoint point)
{
	CView::OnMButtonUp(nFlags, point);
}


void CGRealtimeViewerView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CView::OnRButtonDown(nFlags, point);
}

BOOL CGRealtimeViewerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CGRealtimeViewerView::SetSelection(CRect rectSelected)
{
}

void CGRealtimeViewerView::SetHighlight(CRect rectSelected)
{
}

BOOL CGRealtimeViewerView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGRealtimeViewerView::installTimer(BOOL bAdd)
{
}

void CGRealtimeViewerView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager ()->ShowPopupMenu (IDR_MENU_VIEW, point.x, point.y, this, TRUE);
}

void CGRealtimeViewerView::OnPopupSelection()
{
	m_bSelection ^= true;
}

void CGRealtimeViewerView::OnUpdatePopupSelection(CCmdUI *pCmdUI)
{	
}

void CGRealtimeViewerView::OnPopupRestore()
{	
}

void CGRealtimeViewerView::OnPopupZoomfit()
{
}
*/