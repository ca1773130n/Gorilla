// GRealtimeViewerView.h : interface of the CGRealtimeViewerView class
//


#pragma once

#include <boost/shared_ptr.hpp>
#include <afxmt.h>
#include "GRealtimeViewerDoc.h"
// namespace girl
// {

// wxViewerMode Declarations
enum ViewerMode
{
	STATIC,
	PANZOOM,
	SELECTION
};

#define ID_ANIMATIONUPDATE	3000

class CGRealtimeViewerView : public CView
{
protected: // create from serialization only
	CGRealtimeViewerView();
	DECLARE_DYNCREATE(CGRealtimeViewerView)

// Attributes
public:
	CGRealtimeViewerDoc* GetDocument() const;

	HGLRC			m_hGLRC;
	HDC				m_hDC;
	CRect			m_rectClient;

	int             m_imageW, m_imageH;
	int             m_tilesX, m_tilesY, m_tilesNr;
	bool            m_firstDraw;
	bool            m_imageChanged;
	int				m_textureW;
	int				m_textureH;
	int             m_offsetX, m_offsetY, m_scaleXo2, m_scaleYo2, m_scaleXo, m_scaleYo, m_lastX, m_lastY;
	float           m_scale;
	float           m_scaleExp;
	int             m_lastW, m_lastH;

/*	wxTimer*        m_animTimer;*/
	UINT			m_timerAnimation;
	int             m_stipple;
/*	wxViewerSelection  m_selection;*/
	bool            m_selectionChanged;
/*	wxViewerSelection  m_highlightSel;*/
	bool            m_refreshMarchingAntsOnly;

	ViewerMode		m_viewerMode;
	CRect			m_rectSelected;
	CRect			m_rectHighlightSel;
	bool			m_bLDown;
	bool			m_bMDown;
	bool			m_bSelection;
	bool			m_bRestore;
	bool			m_bZoomFit;

	CPoint			m_ptFirst;
	CPoint			m_ptSecond;
// Operations
public:
	

	void InverseTransformPoint(int x, int y, int &invX, int &invY);
	//void DrawMarchingAnts(/*const wxViewerSelection &selection, */float red, float green, float blue);
	void DrawMarchingAnts(CRect rectSelection, float red, float green, float blue);
	void Reload ();
	void Reset ();
	void SetMode (ViewerMode mode);
	void SetZoom(CRect rectSelected);
	void SetSelection(CRect rectSelected);
	
// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CGRealtimeViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnPaint();
	/*
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnInitialUpdate();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void SetHighlight(CRect rectSelected);
	void installTimer(BOOL bAdd);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnPopupSelection();
	afx_msg void OnContextMenu(CWnd*, CPoint );
	afx_msg void OnUpdatePopupSelection(CCmdUI *pCmdUI);
	afx_msg void OnPopupRestore();
	afx_msg void OnPopupZoomfit();
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	*/
};

// #ifndef _DEBUG  // debug version in GRealtimeViewerView.cpp
// inline CGRealtimeViewerDoc* CGRealtimeViewerView::GetDocument() const
//    { return reinterpret_cast<CGRealtimeViewerDoc*>(m_pDocument); }
// #endif

//}//namespace girl