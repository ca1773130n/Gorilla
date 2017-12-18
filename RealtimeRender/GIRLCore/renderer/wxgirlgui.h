/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   wxgirlgui.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - wxgirlgui.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_WXGIRLGUI_H
#define GIRL_WXGIRLGUI_H

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <string>

#include <wx/scrolwin.h>
#include <wx/progdlg.h>

#include "wxgirlframe.h"
#include "wxviewer.h"

namespace girl
{

#define ID_RENDERUPDATE	2000
#define ID_STATSUPDATE	2001
#define ID_LOADUPDATE		2002

/*** GirlError and wxGirlErrorEvent ***/

class GirlError {
public:
	GirlError(int code, int severity, const char *msg): m_code(code), m_severity(severity), m_msg(msg) {}

	int GetCode() { return m_code; }
	int GetSeverity() { return m_severity; }
	const std::string& GetMessage() { return m_msg; }

protected:
	int m_code;
	int m_severity;
	std::string m_msg;
};

class wxGirlErrorEvent : public wxEvent {
public:
    wxGirlErrorEvent(const boost::shared_ptr<GirlError> error, wxEventType eventType = wxEVT_NULL, int id = 0): wxEvent(id, eventType), m_error(error) {}

    boost::shared_ptr<GirlError> GetError() { return m_error; }

    // required for sending with wxPostEvent()
    wxEvent* Clone(void) const { return new wxGirlErrorEvent(*this); }

protected:
    boost::shared_ptr<GirlError> m_error;
};

DECLARE_EVENT_TYPE(wxEVT_GIRL_ERROR, -1)
DECLARE_EVENT_TYPE(wxEVT_GIRL_PARSEERROR, -1)
DECLARE_EVENT_TYPE(wxEVT_GIRL_FINISHED, -1)
DECLARE_EVENT_TYPE(wxEVT_GIRL_TONEMAPPED, -1)

typedef void (wxEvtHandler::*wxGirlErrorEventFunction)(wxGirlErrorEvent&);

#define EVT_GIRL_ERROR(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_GIRL_ERROR, id, -1, \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
    wxStaticCastEvent( wxGirlErrorEventFunction, & fn ), (wxObject *) NULL ),

/*** GirlOutputWin ***/

class GirlOutputWin : public wxScrolledWindow, public wxViewerBase {
public:
	GirlOutputWin(wxWindow *parent);
	virtual wxWindow* GetWindow();
	virtual void Reload();

protected:
	DECLARE_EVENT_TABLE()
	void OnDraw(wxDC &dc);
};


/*** GirlGui ***/

enum GirlGuiRenderState
{
	WAITING,
	RENDERING,
	STOPPING,
	STOPPED,
	PAUSED,
	FINISHED
};
enum GirlGuiWindowState
{
	SHOWN,
	HIDDEN
};

class GirlGui : public GirlMainFrame {
public:
	/** Constructor */
	GirlGui(wxWindow* parent, bool opengl, bool copylog2console);

	void RenderScenefile(wxString filename);
	void SetRenderThreads(int num);

protected:
	DECLARE_EVENT_TABLE()
	// Handlers for GirlMainFrame events.
	void OnMenu(wxCommandEvent &event);
	void OnOpen(wxCommandEvent &event);
	void OnExit(wxCloseEvent &event);
	void OnError(wxGirlErrorEvent &event);
	void OnTimer(wxTimerEvent& event);
	void OnCommand(wxCommandEvent &event);
	void OnIconize(wxIconizeEvent& event);
	void OnSelection(wxViewerEvent& event);
	
	void ChangeRenderState(GirlGuiRenderState state);
	void LoadImages();

	// Parsing and rendering threads
	void EngineThread(wxString filename);
	void UpdateThread();
	int m_numThreads;

	void UpdateStatistics();

	boost::thread *m_engineThread, *m_updateThread;
	bool m_opengl;
	bool m_copyLog2Console;
	double m_samplesSec;
	GirlGuiRenderState m_guiRenderState;
	GirlGuiWindowState m_guiWindowState;

	wxProgressDialog* m_progDialog;
	wxViewerBase* m_renderOutput;
	wxTimer* m_loadTimer;
	wxTimer* m_renderTimer;
	wxTimer* m_statsTimer;

	wxBitmap m_splashbmp;

	// CF

	class GirlOptions : public m_OptionsDialog {
		public:

		GirlOptions( GirlGui* parent );

		protected:

			void OnMenu( wxCommandEvent& event );
			void OnScroll( wxScrollEvent& event ); 
			void OnText(wxCommandEvent &event);
			void OnClose( wxCloseEvent& event );
			void OnSpin( wxSpinEvent& event );

		public:
			void ResetToneMapping( void );

			double m_RH_pre;
			double m_RH_post;
			double m_RH_burn;

			GirlGui *m_Parent;

			void UpdateSysOptions( void );
			void ApplySysOptions( void );

			unsigned int m_DisplayInterval;
			unsigned int m_WriteInterval;

			bool m_UseFlm;
			bool m_Write_TGA;
			bool m_Write_TM_EXR;
			bool m_Write_UTM_EXR;
			bool m_Write_TM_IGI;
			bool m_Write_UTM_IGI;
	};

	GirlOptions *m_GirlOptions;
};


/*** GirlGuiErrorHandler wrapper ***/

void GirlGuiErrorHandler(int code, int severity, const char *msg);

}//namespace girl

#endif // GIRL_WXGIRLGUI_H
