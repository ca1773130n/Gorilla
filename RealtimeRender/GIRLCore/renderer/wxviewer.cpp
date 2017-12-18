/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   wxviewer.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - wxviewer.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "wxviewer.h"

using namespace girl;


// wxViewerSelection Definitions
wxViewerSelection::wxViewerSelection(int x1, int x2, int y1, int y2)
	: m_x1(x1), m_x2(x2), m_y1(y1), m_y2(y2) {}

void wxViewerSelection::GetBounds(int &x1, int &x2, int &y1, int &y2) const {
	x1 = m_x1;
	x2 = m_x2;
	y1 = m_y1;
	y2 = m_y2;
}

void wxViewerSelection::SetBounds(int x1, int x2, int y1, int y2) {
	m_x1 = x1;
	m_x2 = x2;
	m_y1 = y1;
	m_y2 = y2;
}


// wxViewerEvent Definitions
DEFINE_EVENT_TYPE(girl::wxEVT_GIRL_VIEWER_SELECTION)

wxViewerEvent::wxViewerEvent(const boost::shared_ptr<wxViewerSelection> selection, wxEventType eventType, int id): wxEvent(id, eventType), m_selection(selection) { m_propagationLevel = wxEVENT_PROPAGATE_MAX; }

boost::shared_ptr<wxViewerSelection> wxViewerEvent::GetSelection() { return m_selection; }

wxEvent* wxViewerEvent::Clone() const { return new wxViewerEvent(*this); }


// wxViewerBase Definitions
wxViewerBase::wxViewerBase():m_viewerMode(STATIC) {}
wxViewerBase::~wxViewerBase() {}

wxViewerSelection wxViewerBase::GetSelection() { return wxViewerSelection(); }

void wxViewerBase::SetMode(wxViewerMode mode) { m_viewerMode = mode; }
void wxViewerBase::SetZoom(const wxViewerSelection *selection) {}
void wxViewerBase::SetSelection(const wxViewerSelection *selection) {}
void wxViewerBase::SetHighlight(const wxViewerSelection *selection) {}

void wxViewerBase::Reload() {}
void wxViewerBase::Reset() {}
