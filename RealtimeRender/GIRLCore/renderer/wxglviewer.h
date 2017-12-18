/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   wxglviewer.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - wxglviewer.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_WXGLVIEWER_H
#define GIRL_WXGLVIEWER_H

#include "wxviewer.h"

#ifdef GIRL_USE_OPENGL
#include "wx/wx.h"
#include "wx/glcanvas.h"
#endif // GIRL_USE_OPENGL

namespace girl
{

#ifdef GIRL_USE_OPENGL

//  [8/20/2008 Sung-Soo Kim] insert include statement.
#include <wx/msw/glcanvas.h>

#define ID_ANIMATIONUPDATE	3000

class GirlGLViewer : public wxGLCanvas, public wxViewerBase {
public:
	GirlGLViewer(wxWindow *parent, int textureW = 256, int textureH = 256);

protected:
	DECLARE_EVENT_TABLE()
	void OnPaint(wxPaintEvent &event);
	void OnEraseBackground(wxEraseEvent &event);
	void OnSize(wxSizeEvent &event);
	void OnMouse(wxMouseEvent &event);
	void OnTimer(wxTimerEvent &event);

	// wxViewerBase methods
	virtual wxWindow* GetWindow();
	virtual wxViewerSelection GetSelection();
	virtual void SetMode(wxViewerMode mode);
	virtual void SetZoom(const wxViewerSelection *selection);
	virtual void SetSelection(const wxViewerSelection *selection);
	virtual void SetHighlight(const wxViewerSelection *selection);
	virtual void Reload();
	virtual void Reset();

	void InverseTransformPoint(int x, int y, int &invX, int &invY);
	void DrawMarchingAnts(const wxViewerSelection &selection, float red, float green, float blue);

	wxGLContext        m_glContext;

	int                m_imageW, m_imageH;
	int                m_tilesX, m_tilesY, m_tilesNr;
	bool               m_firstDraw;
	bool               m_imageChanged;
	const int          m_textureW;
	const int          m_textureH;
	int                m_offsetX, m_offsetY, m_scaleXo2, m_scaleYo2, m_scaleXo, m_scaleYo, m_lastX, m_lastY;
	float              m_scale;
	float              m_scaleExp;
	int                m_lastW, m_lastH;

	wxTimer*           m_animTimer;
	int                m_stipple;
	wxViewerSelection  m_selection;
	bool               m_selectionChanged;
	wxViewerSelection  m_highlightSel;
	bool               m_refreshMarchingAntsOnly;
};

#else // GIRL_USE_OPENGL

//dummy class
class GirlGLViewer : public wxWindow, public wxViewerBase {
public:
	GirlGLViewer(wxWindow *parent, int textureW = 256, int textureH = 256) {}
	virtual wxWindow* GetWindow() { return this; }
};

#endif // GIRL_USE_OPENGL

}//namespace girl

#endif // GIRL_WXGLVIEWER_H
