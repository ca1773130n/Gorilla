/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   wxglviewer.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - wxglviewer.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifdef GIRL_USE_OPENGL

#include "wxglviewer.h"

// include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include <cmath>

#include "girl.h"
#include "api.h"

using namespace girl;

BEGIN_EVENT_TABLE(GirlGLViewer, wxWindow)
		EVT_PAINT            (GirlGLViewer::OnPaint)
		EVT_SIZE             (GirlGLViewer::OnSize)
		EVT_MOUSE_EVENTS     (GirlGLViewer::OnMouse)
		EVT_ERASE_BACKGROUND (GirlGLViewer::OnEraseBackground)
		EVT_TIMER            (wxID_ANY, GirlGLViewer::OnTimer)
END_EVENT_TABLE()

int glAttribList[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0};

GirlGLViewer::GirlGLViewer(wxWindow *parent, int textureW, int textureH)
#ifdef __WXMAC__
      : wxGLCanvas(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxGLCanvasName, glAttribList), wxViewerBase(), m_glContext(NULL,this,wxNullPalette,NULL), m_textureW(textureW), m_textureH(textureH){
#else
      : wxGLCanvas(parent, wxID_ANY, glAttribList), wxViewerBase(), m_glContext(this), m_textureW(textureW), m_textureH(textureH) {
#endif
	m_firstDraw = true;
	m_offsetX = m_offsetY = 0;
	m_scale = 1.0f;
	m_scaleExp=0;
	m_scaleXo = m_scaleYo = 0;
	m_scaleXo2 = m_scaleYo2 = 0;
	m_lastX = m_lastY = 0;

	m_stipple = 0x00FF; // Stipple pattern - dashes
	m_animTimer = new wxTimer(this, ID_ANIMATIONUPDATE);
	m_animTimer->Start(125, wxTIMER_CONTINUOUS); // Animation at 8fps

	m_selectionChanged = false;
	m_refreshMarchingAntsOnly = false;

	SetMode(PANZOOM);

	SetCursor(wxCURSOR_CROSS);
}

void GirlGLViewer::OnPaint(wxPaintEvent& event) {
#ifdef __WXMAC__
	SetCurrent();
#else
	SetCurrent(m_glContext);
#endif
	wxPaintDC(this);

	if (!m_refreshMarchingAntsOnly) {
		glClearColor(0.5, 0.5, 0.5, 1.0);
	}
	glViewport(0, 0, (GLint)GetSize().x, (GLint)GetSize().y);
	glLoadIdentity();
	glOrtho(0, GetSize().x, 0, GetSize().y, -1, 1);
	if (!m_refreshMarchingAntsOnly) {
		glClear(GL_COLOR_BUFFER_BIT);
	}

	if(girlStatistics("sceneIsReady")) {
		if(m_firstDraw) {
			m_firstDraw = false;
			m_imageW = girlStatistics("filmXres");
			m_imageH = girlStatistics("filmYres");

			// NOTE - Ratow - using ceiling function so that a minimum number of tiles are created in the case of m_imageW % 256 == 0
			m_tilesX = ceil((float)m_imageW/m_textureW);
			m_tilesY = ceil((float)m_imageH/m_textureH);
			m_tilesNr = m_tilesX*m_tilesY;
			for(int i = 0; i < m_tilesNr; i++){
				glBindTexture(GL_TEXTURE_2D, i+1);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glPixelStorei(GL_UNPACK_ROW_LENGTH, m_imageW);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_LINEAR); //'linear' causes seams to show on my ati card - zcott
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_textureW, m_textureH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); //warning: the texture isn't initialized, don't display before uploading
			}
			m_imageChanged = true;
			//move to center of window
			m_offsetX = m_offsetY = 0;
			m_scale = 1.0f;
			m_scaleExp=0;
			m_scaleXo = m_scaleYo = 0;
			m_lastX = m_lastY = 0;
			m_scaleXo2 = -m_imageW/2 + GetSize().x/2;
			m_scaleYo2 = -m_imageH/2 + GetSize().y/2;
		}

		glPushMatrix();
		//transform and roll out!
		glTranslatef(m_offsetX, m_offsetY, 0.f);
		glTranslatef(m_scaleXo2, m_scaleYo2, 0.f);
		glScalef(m_scale, m_scale, 1.f);
		glTranslatef(-m_scaleXo, -m_scaleYo, 0.f);

		if (!m_refreshMarchingAntsOnly) {
			glEnable (GL_TEXTURE_2D);
			//draw the texture tiles
			for(int y = 0; y < m_tilesY; y++){
				for(int x = 0; x < m_tilesX; x++){
					int offX = x*m_textureW;
					int offY = y*m_textureH;
					int tileW = min(m_textureW, m_imageW - offX);
					int tileH = min(m_textureH, m_imageH - offY);
					glBindTexture (GL_TEXTURE_2D, y*m_tilesX+x+1);
					if(m_imageChanged)	{ //upload the textures only when needed (takes long...)
						// NOTE - Ratow - loading texture tile in one pass
						glPixelStorei(GL_UNPACK_SKIP_PIXELS, offX);
						glPixelStorei(GL_UNPACK_SKIP_ROWS, offY);
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tileW, tileH, GL_RGB, GL_UNSIGNED_BYTE, girlFramebuffer());
					}
					glBegin(GL_QUADS);
					glTexCoord2f(                 0.0f,                             0.0f);
					glVertex3f(  x*m_textureW +   0.0f, m_imageH -  (y*m_textureH + 0.0f), 0.0f);
					glTexCoord2f(1.0f*tileW/m_textureW,                             0.0f);
					glVertex3f(  x*m_textureW +  tileW, m_imageH -  (y*m_textureH + 0.0f), 0.0f);
					glTexCoord2f(1.0f*tileW/m_textureW,            1.0f*tileH/m_textureH);
					glVertex3f(  x*m_textureW +  tileW, m_imageH - (y*m_textureH + tileH), 0.0f);
					glTexCoord2f(                 0.0f,            1.0f*tileH/m_textureH);
					glVertex3f(  x*m_textureW +   0.0f, m_imageH - (y*m_textureH + tileH), 0.0f);
					glEnd();
				}
			}
			glDisable(GL_TEXTURE_2D);
		}

		if (m_selection.HasSize()) {
			// Draw current (white) selection area
			DrawMarchingAnts(m_selection, 1.0, 1.0, 1.0); 
		}
		if (m_highlightSel.HasSize()) {
			// Draw active (red) work area
			DrawMarchingAnts(m_highlightSel, 1.0, 0.0, 0.0); 
		}

		glPopMatrix();
	}

	glFlush();
	SwapBuffers();
	m_imageChanged = false;
	m_refreshMarchingAntsOnly = false;
}

void GirlGLViewer::OnEraseBackground(wxEraseEvent &event) {
	// Method overridden for less flicker:
	// http://wiki.wxwidgets.org/WxGLCanvas#Animation_flicker:_wxGLCanvas-as-member_vs_subclass-as-member
	return;
}

void GirlGLViewer::OnSize(wxSizeEvent &event) {
	wxGLCanvas::OnSize(event);

	int W, H;
	W = event.GetSize().x;
	H = event.GetSize().y;

	if(!m_firstDraw) {
		//calculate new offset
		m_offsetX += (W-m_lastW)/2;
		m_offsetY += (H-m_lastH)/2;
		//make sure new offset is in bounds
		if((m_imageW-m_scaleXo)*m_scale+m_scaleXo2+m_offsetX-10<0)
			m_offsetX = (int)(     10-m_scaleXo2-(m_imageW-m_scaleXo)*m_scale );
		if((0-m_scaleXo)*m_scale+m_scaleXo2+m_offsetX+10>W-1)
			m_offsetX = (int)( W-1-10-m_scaleXo2-(0-m_scaleXo)*m_scale );
		if((m_imageH-m_scaleYo)*m_scale+m_scaleYo2+m_offsetY-10<0)
			m_offsetY = (int)(     10-m_scaleYo2-(m_imageH-m_scaleYo)*m_scale );
		if((0-m_scaleYo)*m_scale+m_scaleYo2+m_offsetY+10>H-1)
			m_offsetY = (int)( H-1-10-m_scaleYo2-(0-m_scaleYo)*m_scale );
	}
	m_lastW = W;
	m_lastH = H;

	Update();
}

void GirlGLViewer::OnMouse(wxMouseEvent &event) {
	int W, H;
	GetClientSize(&W, &H);

	if(m_viewerMode == PANZOOM) {
		// Skip events if we have nothing to draw
		if(m_firstDraw) {
			event.Skip();
		} else if(event.GetEventType() == wxEVT_LEFT_DOWN) {
			SetCursor(wxCURSOR_HAND);
			event.Skip();
		} else if(event.GetEventType() == wxEVT_LEFT_UP) {
			SetCursor(wxCURSOR_CROSS);
		} else if(event.GetEventType() == wxEVT_MIDDLE_DOWN) {
			m_scale = min(W/(float)m_imageW, H/(float)m_imageH);
			m_scaleExp = floor(log((float)m_scale)/log(2.0f)*2+0.5f)/2;
			m_scaleXo2 = W/2;
			m_scaleYo2 = H/2;
			m_scaleXo = m_imageW/2;
			m_scaleYo = m_imageH/2;
			m_offsetX = 0;
			m_offsetY = 0;
			Refresh();
		} else if(event.GetEventType() == wxEVT_RIGHT_DOWN) {
			//calculate the scaling point in image space
			m_scaleXo = (int)(     event.GetX() /m_scale-m_scaleXo2/m_scale-m_offsetX/m_scale+m_scaleXo);
			m_scaleYo = (int)((H-1-event.GetY())/m_scale-m_scaleYo2/m_scale-m_offsetY/m_scale+m_scaleYo);
			//make sure the scaling point is in bounds
			if(m_scaleXo<0) m_scaleXo = 0;
			if(m_scaleXo>m_imageW-1) m_scaleXo = m_imageW-1;
			if(m_scaleYo<0) m_scaleYo = 0;
			if(m_scaleYo>m_imageH-1) m_scaleYo = m_imageH-1;
			//new scale
			m_scaleExp = 0;
			m_scale = 1.0f;
			//get the scaling point in window space
			m_scaleXo2 =     event.GetX();
			m_scaleYo2 = H-1-event.GetY();
			m_offsetX = m_offsetY = 0;
			Refresh();
		} else if(event.GetEventType() == wxEVT_MOTION) {
			if(event.Dragging() && event.m_leftDown) {
				//calculate new offset
				int offsetXNew = m_offsetX + (event.GetX()-m_lastX);
				int offsetYNew = m_offsetY - (event.GetY()-m_lastY);
				//check if new offset in bounds
				if((m_imageW-m_scaleXo)*m_scale+m_scaleXo2+offsetXNew-10>=0 && (0-m_scaleXo)*m_scale+m_scaleXo2+offsetXNew+10<=W-1)
					m_offsetX = offsetXNew;
				if((m_imageH-m_scaleYo)*m_scale+m_scaleYo2+offsetYNew-10>=0 && (0-m_scaleYo)*m_scale+m_scaleYo2+offsetYNew+10<=H-1)
					m_offsetY = offsetYNew;
				Refresh();
			}
		} else if(event.GetEventType() == wxEVT_MOUSEWHEEL) {
			if((event.GetWheelRotation() > 0 && m_scale < 4) || //zoom in up to 4x
					(event.GetWheelRotation() < 0 && (m_scale > 1 || m_scale*m_imageW > W*0.5f || m_scale*m_imageH > H*0.5f))) { //zoom out up to 50% of window size
				//calculate the scaling point in image space
				m_scaleXo = (int)(     event.GetX() /m_scale-m_scaleXo2/m_scale-m_offsetX/m_scale+m_scaleXo);
				m_scaleYo = (int)((H-1-event.GetY())/m_scale-m_scaleYo2/m_scale-m_offsetY/m_scale+m_scaleYo);
				//make sure the scaling point is in bounds
				if(m_scaleXo < 0) m_scaleXo = 0;
				if(m_scaleXo > m_imageW-1) m_scaleXo = m_imageW-1;
				if(m_scaleYo < 0) m_scaleYo = 0;
				if(m_scaleYo > m_imageH-1) m_scaleYo = m_imageH-1;
				//calculate new scale
				m_scaleExp += (event.GetWheelRotation()>0?1:-1)*0.5f;
				m_scale = pow(2.0f, m_scaleExp);
				//get the scaling point in window space
				m_scaleXo2 = event.GetX();
				m_scaleYo2 = H-1-event.GetY();
				m_offsetX = m_offsetY = 0;
				Refresh();
			}
		} else {
			event.Skip();
		}
	} else if(m_viewerMode == SELECTION) {
		if(event.GetEventType() == wxEVT_LEFT_DOWN) {
			int x, y;
			InverseTransformPoint(event.GetX(), event.GetY(), x, y);
			m_selection.SetBounds(x, x, y, y);
		} else if(event.GetEventType() == wxEVT_LEFT_UP) {
			boost::shared_ptr<wxViewerSelection> selection(new wxViewerSelection(m_selection));
			int x, y;
			InverseTransformPoint(event.GetX(), event.GetY(), x, y);
			selection->SetCorner2(x, y);
			wxViewerEvent viewerEvent(selection, wxEVT_GIRL_VIEWER_SELECTION);
			GetEventHandler()->AddPendingEvent(viewerEvent);
		} else if(event.GetEventType() == wxEVT_MOTION) {
			if(event.Dragging() && event.m_leftDown) {
				int oldX, oldY, newX, newY;
				m_selection.GetCorner2(oldX, oldY);
				InverseTransformPoint(event.GetX(), event.GetY(), newX, newY);
				if ((newX != oldX) || (newY != oldY)) {
					m_selection.SetCorner2(newX, newY);
					m_selectionChanged = true;
				}
			}
		}
	}
	m_lastX = event.GetX();
	m_lastY = event.GetY();
}

wxWindow* GirlGLViewer::GetWindow() {
	return this;
}

wxViewerSelection GirlGLViewer::GetSelection() {
	return m_selection;
}

void GirlGLViewer::SetMode(wxViewerMode mode) {
	wxViewerBase::SetMode(mode);
	Refresh();
}

void GirlGLViewer::SetZoom(const wxViewerSelection *selection) {
	int W, H;
	GetClientSize(&W, &H);
	int x1, x2, y1, y2;
	selection->GetBounds(x1,x2,y1,y2);
	int selMaxX = min(m_imageW, max(x1, x2));
	int selMaxY = min(m_imageH, max(y1, y2));
	int selMinX = max(0, min(x1, x2));
	int selMinY = max(0, min(y1, y2));
	float exactScale = min((float)W/(selMaxX-selMinX), (float)H/(selMaxY-selMinY));

	if(isinf(exactScale)) // If selection is too small, increase it to 1 pixel
		exactScale = min(W, H);

	// Zoom in and center selection
	m_scaleXo = (selMaxX + selMinX)/2.0;
	m_scaleYo = (selMaxY + selMinY)/2.0;
	m_scaleExp = log(exactScale)/log(2.0);
	m_scale = exactScale;
	m_scaleXo2 = W/2.0;
	m_scaleYo2 = H/2.0-1;
	m_offsetX = m_offsetY = 0;
	Refresh();
}

void GirlGLViewer::SetSelection(const wxViewerSelection *selection) {
	if (!selection) {
		m_selection.Clear();
	} else {
		m_selection = *selection;
	}
	Refresh();
}

void GirlGLViewer::SetHighlight(const wxViewerSelection *selection) {
	if (!selection) {
		m_highlightSel.Clear();
	} else {
		m_highlightSel = *selection;
	}
	Refresh();
}

void GirlGLViewer::Reload() {
	m_imageChanged = true;
	Refresh();
}

void GirlGLViewer::Reset() {
	m_firstDraw = true;
	Refresh();
}

void GirlGLViewer::OnTimer(wxTimerEvent &event) {
	if (!m_selection.HasSize() && !m_highlightSel.HasSize())	return;

	m_stipple = ((m_stipple >> 15) | (m_stipple << 1)) & 0xFFFF;
	if (!m_selectionChanged) {
		m_refreshMarchingAntsOnly = true;
	}
	m_selectionChanged = false;
	this->Refresh();
}

void GirlGLViewer::InverseTransformPoint(int x, int y, int &invX, int &invY) {
	int width, height;
	this->GetClientSize(&width, &height);

	invX = (x - m_offsetX - m_scaleXo2)/m_scale + m_scaleXo;
	invY = (height - y - m_offsetY - m_scaleYo2)/m_scale + m_scaleYo;
}

void GirlGLViewer::DrawMarchingAnts(const wxViewerSelection &selection, float red, float green, float blue) {
	int x1, x2, y1, y2;
	selection.GetBounds(x1, x2, y1, y2);

	glEnable(GL_LINE_STIPPLE);

	glLineWidth(2.0); // Perhaps we should change width according to zoom

	glLineStipple(1, m_stipple);
	glBegin(GL_LINE_STRIP);
	glColor3f(0.0, 0.0, 0.0);
	glVertex3i(x1, y1, 0.0);
	glVertex3i(x1, y2, 0.0);
	glVertex3i(x2, y2, 0.0);
	glVertex3i(x2, y1, 0.0);
	glVertex3i(x1, y1, 0.0);
	glEnd();

	glLineStipple(1, ~m_stipple);
	glBegin(GL_LINE_STRIP);
	glColor3f(red, green, blue);
	glVertex3i(x1, y1, 0.0);
	glVertex3i(x1, y2, 0.0);
	glVertex3i(x2, y2, 0.0);
	glVertex3i(x2, y1, 0.0);
	glVertex3i(x1, y1, 0.0);
	glEnd();

	glDisable(GL_LINE_STIPPLE);
}

#endif // GIRL_USE_OPENGL
