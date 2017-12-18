/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   wxviewer.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - wxviewer.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_WXVIEWER_H
#define GIRL_WXVIEWER_H

#include <boost/shared_ptr.hpp>
#include <wx/event.h>
#include <wx/window.h>

namespace girl
{

// wxViewerSelection Declarations - Used for zoom and other tools
class wxViewerSelection {
public:
	inline wxViewerSelection(void)  { Clear(); }
	wxViewerSelection(int x1, int x2, int y1, int y2);
	inline void Clear(void)  { m_x1 = m_x2 = m_y1 = m_y2 = 0; }
	void GetBounds(int &x1, int &x2, int &y1, int &y2) const;
	void SetBounds(int x1, int x2, int y1, int y2);
	inline void GetCorner1(int &x, int &y) const  { x = m_x1;  y = m_y1; }
	inline void GetCorner2(int &x, int &y) const  { x = m_x2;  y = m_y2; }
	inline void SetCorner1(int &x, int &y)  { m_x1 = x;  m_y1 = y; }
	inline void SetCorner2(int &x, int &y)  { m_x2 = x;  m_y2 = y; }
	inline bool HasSize(void) const  { return (m_x1 != m_x2) && (m_y1 != m_y2); }

protected:
	int m_x1, m_x2, m_y1, m_y2;
};

// wxViewerEvent Declarations
class wxViewerEvent : public wxEvent {
public:
    wxViewerEvent(const boost::shared_ptr<wxViewerSelection> selection, wxEventType eventType = wxEVT_NULL, int id = 0);

		boost::shared_ptr<wxViewerSelection> GetSelection();

    // required for sending with wxPostEvent()
    wxEvent* Clone() const;

protected:
    boost::shared_ptr<wxViewerSelection> m_selection;
};

DECLARE_EVENT_TYPE(wxEVT_GIRL_VIEWER_SELECTION, -1)

typedef void (wxEvtHandler::*wxViewerEventFunction)(wxViewerEvent&);

#define EVT_GIRL_VIEWER_SELECTION(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_GIRL_VIEWER_SELECTION, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( wxViewerEventFunction, & fn ), (wxObject *) NULL ),

// wxViewerMode Declarations
enum wxViewerMode
{
	STATIC,
	PANZOOM,
	SELECTION
};

// wxViewerBase Declarations
class wxViewerBase {
public:
	wxViewerBase();
	virtual ~wxViewerBase();

	virtual wxWindow* GetWindow() = 0;
	virtual wxViewerSelection GetSelection();

	virtual void SetMode(wxViewerMode mode);
	virtual void SetZoom(const wxViewerSelection *selection);
	virtual void SetSelection(const wxViewerSelection *selection);
	virtual void SetHighlight(const wxViewerSelection *selection);

	virtual void Reload();
	virtual void Reset();

protected:
	wxViewerMode m_viewerMode;
};

}//namespace girl

#endif // GIRL_WXGIRLGUI_H
