/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   wxgirlgui.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - wxgirlgui.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#include <boost/bind.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/thread.hpp>
#include <boost/cast.hpp>

#include <sstream>
#include <clocale>

#include "girl.h"
#include "api.h"
#include "error.h"

#include "wx/app.h"
#include "wx/filedlg.h"
#include "wx/filename.h"
#include "wx/dcbuffer.h"
#include "wx/splash.h"
#include "wx/clipbrd.h" // CF
#include <boost/date_time/posix_time/posix_time.hpp>

#include "wxgirlgui.h"
#include "wxglviewer.h"
#include "wximages.h"

using namespace girl;

/*** GirlGui ***/

//////////////////////////////////////////////////////////////////////////////////////////////////
// CF

#define RH_PRE_RANGE	8.0f
#define RH_POST_RANGE	6.0f
#define RH_BURN_RANGE	10.0f

//////////////////////////////////////////////////////////////////////////////////////////////////

DEFINE_EVENT_TYPE(girl::wxEVT_GIRL_ERROR)
DEFINE_EVENT_TYPE(girl::wxEVT_GIRL_PARSEERROR)
DEFINE_EVENT_TYPE(girl::wxEVT_GIRL_FINISHED)
DEFINE_EVENT_TYPE(girl::wxEVT_GIRL_TONEMAPPED)

BEGIN_EVENT_TABLE(GirlGui, wxFrame)
	EVT_GIRL_ERROR (wxID_ANY, GirlGui::OnError)
	EVT_GIRL_VIEWER_SELECTION (wxID_ANY, GirlGui::OnSelection)
	EVT_TIMER     (wxID_ANY, GirlGui::OnTimer)
	EVT_COMMAND   (wxID_ANY, girl::wxEVT_GIRL_TONEMAPPED, GirlGui::OnCommand)
	EVT_COMMAND   (wxID_ANY, girl::wxEVT_GIRL_PARSEERROR, GirlGui::OnCommand)
	EVT_COMMAND   (wxID_ANY, girl::wxEVT_GIRL_FINISHED, GirlGui::OnCommand)
	EVT_ICONIZE   (GirlGui::OnIconize)
END_EVENT_TABLE()

// Dade - global variable used by GirlGuiErrorHandler()
bool copyLog2Console = false;

GirlGui::GirlGui(wxWindow* parent, bool opengl, bool copylog2console) :
	GirlMainFrame(parent), m_opengl(opengl), m_copyLog2Console(copylog2console) {
	// Load images and icons from header.
	LoadImages();

	// Add custom output viewer window
	if(m_opengl)
		m_renderOutput = new GirlGLViewer(m_renderPage);
	else
		m_renderOutput = new GirlOutputWin(m_renderPage);
	m_renderPage->GetSizer()->Add(m_renderOutput->GetWindow(), 1, wxALL | wxEXPAND, 5);
	m_renderPage->Layout();

	// Trick to generate resize event and show output window
	// http://lists.wxwidgets.org/pipermail/wx-users/2007-February/097829.html
	SetSize(GetSize());
	m_renderOutput->GetWindow()->Update();

	// Create render output update timer
	m_renderTimer = new wxTimer(this, ID_RENDERUPDATE);
	m_statsTimer = new wxTimer(this, ID_STATSUPDATE);
	m_loadTimer = new wxTimer(this, ID_LOADUPDATE);

	m_numThreads = 0;
	m_engineThread = NULL;
	m_updateThread = NULL;

	// Dade - I should use boost bind to avoid global variable
	copyLog2Console = m_copyLog2Console;
	girlErrorHandler(&GirlGuiErrorHandler);

	ChangeRenderState(WAITING);
	m_guiWindowState = SHOWN;

	// CF
	m_GirlOptions = new GirlOptions( this );
}

void GirlGui::ChangeRenderState(GirlGuiRenderState state) {
	switch(state) {
		case WAITING:
			// Waiting for input file. Most controls disabled.
			m_render->Enable(ID_RESUMEITEM, false);
			m_render->Enable(ID_PAUSEITEM, false);
			m_render->Enable(ID_STOPITEM, false);
			m_view->Enable(ID_RENDER_COPY, false);
			m_renderToolBar->EnableTool(ID_RESUMETOOL, false);
			m_renderToolBar->EnableTool(ID_PAUSETOOL, false);
			m_renderToolBar->EnableTool(ID_STOPTOOL, false);
			m_renderToolBar->EnableTool(ID_RENDER_COPY, false);
			m_viewerToolBar->Disable();
			break;
		case RENDERING:
			// Rendering is in progress.
			m_render->Enable(ID_RESUMEITEM, false);
			m_render->Enable(ID_PAUSEITEM, true);
			m_render->Enable(ID_STOPITEM, true);
			m_view->Enable(ID_RENDER_COPY, true);
			m_renderToolBar->EnableTool(ID_RESUMETOOL, false);
			m_renderToolBar->EnableTool(ID_PAUSETOOL, true);
			m_renderToolBar->EnableTool(ID_STOPTOOL, true);
			m_renderToolBar->EnableTool(ID_RENDER_COPY, true);
			m_viewerToolBar->Enable();
			break;
		case STOPPING:
			// Rendering is being stopped.
			m_render->Enable(ID_RESUMEITEM, false);
			m_render->Enable(ID_PAUSEITEM, false);
			m_render->Enable(ID_STOPITEM, false);
			m_view->Enable(ID_RENDER_COPY, true);
			m_renderToolBar->EnableTool(ID_RESUMETOOL, false);
			m_renderToolBar->EnableTool(ID_PAUSETOOL, false);
			m_renderToolBar->EnableTool(ID_STOPTOOL, false);
			m_renderToolBar->EnableTool(ID_RENDER_COPY, true);
			break;
		case STOPPED:
			// Rendering is stopped.
			m_render->Enable(ID_RESUMEITEM, true);
			m_render->Enable(ID_PAUSEITEM, false);
			m_render->Enable(ID_STOPITEM, false);
			m_view->Enable(ID_RENDER_COPY, true);
			m_renderToolBar->EnableTool(ID_RESUMETOOL, true);
			m_renderToolBar->EnableTool(ID_PAUSETOOL, false);
			m_renderToolBar->EnableTool(ID_STOPTOOL, false);
			m_renderToolBar->EnableTool(ID_RENDER_COPY, true);
			break;
		case PAUSED:
			// Rendering is paused.
			m_render->Enable(ID_RESUMEITEM, true);
			m_render->Enable(ID_PAUSEITEM, false);
			m_render->Enable(ID_STOPITEM, true);
			m_view->Enable(ID_RENDER_COPY, true);
			m_renderToolBar->EnableTool(ID_RESUMETOOL, true);
			m_renderToolBar->EnableTool(ID_PAUSETOOL, false);
			m_renderToolBar->EnableTool(ID_STOPTOOL, true);
			m_renderToolBar->EnableTool(ID_RENDER_COPY, true);
			break;
		case FINISHED:
			// Rendering is finished.
			m_render->Enable(ID_RESUMEITEM, false);
			m_render->Enable(ID_PAUSEITEM, false);
			m_render->Enable(ID_STOPITEM, false);
			m_view->Enable(ID_RENDER_COPY, true);
			m_renderToolBar->EnableTool(ID_RESUMETOOL, false);
			m_renderToolBar->EnableTool(ID_PAUSETOOL, false);
			m_renderToolBar->EnableTool(ID_STOPTOOL, false);
			m_renderToolBar->EnableTool(ID_RENDER_COPY, true);
			break;
	}
	m_guiRenderState = state;
}

void GirlGui::LoadImages() {
	wxImage::AddHandler(new wxPNGHandler());

	// App icon - only set on non-windows platforms
#ifndef __WXMSW__
	wxIcon appIcon;
	appIcon.CopyFromBitmap(wxMEMORY_BITMAP(girlicon_png));
	SetIcon(appIcon);
#endif

	// wxMac has problems changing an existing tool's icon, so we remove and add then again...
	// Resume toolbar tool
	wxToolBarToolBase *rendertool = m_renderToolBar->RemoveTool(ID_RESUMETOOL);
	rendertool->SetNormalBitmap(wxMEMORY_BITMAP(resume_png));
	m_renderToolBar->InsertTool(0, rendertool);
	m_renderToolBar->Realize();

	// Pause toolbar tool
	wxToolBarToolBase *pausetool = m_renderToolBar->RemoveTool(ID_PAUSETOOL);
	pausetool->SetNormalBitmap(wxMEMORY_BITMAP(pause_png));
	m_renderToolBar->InsertTool(1, pausetool);
	m_renderToolBar->Realize();

	// Stop toolbar tool
	wxToolBarToolBase *stoptool = m_renderToolBar->RemoveTool(ID_STOPTOOL);
	stoptool->SetNormalBitmap(wxMEMORY_BITMAP(stop_png));
	m_renderToolBar->InsertTool(2, stoptool);
	m_renderToolBar->Realize();

	// Add Thread toolbar tool
	// CF
	wxToolBarToolBase *addtheadtool = m_renderToolBar->RemoveTool(ID_ADD_THREAD);
	addtheadtool->SetNormalBitmap(wxMEMORY_BITMAP(plus_png));
	m_renderToolBar->InsertTool(5, addtheadtool);
	m_renderToolBar->Realize();

	// Add Thread toolbar tool
	// CF
	wxToolBarToolBase *remtheadtool = m_renderToolBar->RemoveTool(ID_REMOVE_THREAD);
	remtheadtool->SetNormalBitmap(wxMEMORY_BITMAP(minus_png));
	m_renderToolBar->InsertTool(6, remtheadtool);
	m_renderToolBar->Realize();

	// Copy toolbar tool
	// CF
	wxToolBarToolBase *copytool = m_renderToolBar->RemoveTool(ID_RENDER_COPY);
	copytool->SetNormalBitmap(wxMEMORY_BITMAP(edit_copy_png));
	m_renderToolBar->InsertTool(8, copytool);
	m_renderToolBar->Realize();


	// Pan toolbar tool
	wxToolBarToolBase *pantool = m_viewerToolBar->RemoveTool(ID_PANTOOL);
	pantool->SetNormalBitmap(wxMEMORY_BITMAP(pan_png));
	m_viewerToolBar->InsertTool(0, pantool);
	m_viewerToolBar->Realize();

	// Zoom toolbar tool
	wxToolBarToolBase *zoomtool = m_viewerToolBar->RemoveTool(ID_ZOOMTOOL);
	zoomtool->SetNormalBitmap(wxMEMORY_BITMAP(zoom_png));
	m_viewerToolBar->InsertTool(1, zoomtool);
	m_viewerToolBar->Realize();

	// Refine toolbar tool
	wxToolBarToolBase *refinetool = m_viewerToolBar->RemoveTool(ID_REFINETOOL);
	refinetool->SetNormalBitmap(wxMEMORY_BITMAP(radiofocus_png));
	m_viewerToolBar->InsertTool(2, refinetool);
	m_viewerToolBar->Realize();

	// NOTE - Ratow - Temporarily disabling icons on menu items on the Windows platform.
#ifndef __WXMSW__
	// wxGTK has problems changing an existing menu item's icon, so we remove and add then again...
	// Resume menu item
	wxMenuItem *renderitem = m_render->Remove(ID_RESUMEITEM);
	renderitem->SetBitmap(wxMEMORY_BITMAP(resume_png));
	m_render->Insert(0,renderitem);
	// Pause menu item
	wxMenuItem *pauseitem = m_render->Remove(ID_PAUSEITEM);
	pauseitem->SetBitmap(wxMEMORY_BITMAP(pause_png));
	m_render->Insert(1,pauseitem);
	// stop menu item
	wxMenuItem *stopitem = m_render->Remove(ID_STOPITEM);
	stopitem->SetBitmap(wxMEMORY_BITMAP(stop_png));
	m_render->Insert(2,stopitem);
#endif

	m_auinotebook->SetPageBitmap(0, wxMEMORY_BITMAP(render_png));
	m_auinotebook->SetPageBitmap(1, wxMEMORY_BITMAP(info_png));
	m_auinotebook->SetPageBitmap(2, wxMEMORY_BITMAP(output_png));

	m_splashbmp = wxMEMORY_BITMAP(splash_png);
}

void GirlGui::OnMenu(wxCommandEvent& event) {
	switch (event.GetId()) {
		case ID_RESUMEITEM:
		case ID_RESUMETOOL:
			if(m_guiRenderState != RENDERING) {
				// CF
				m_GirlOptions->UpdateSysOptions();

				// Start display update timer
				m_renderOutput->Reload();
				m_renderTimer->Start(1000*girlStatistics("displayInterval"), wxTIMER_CONTINUOUS);
				m_statsTimer->Start(1000, wxTIMER_CONTINUOUS);
				if(m_guiRenderState == PAUSED || m_guiRenderState == STOPPED) // Only re-start if we were previously stopped
					girlStart();
				if(m_guiRenderState == STOPPED)
					girlSetHaltSamplePerPixel(-1, false, false);
				ChangeRenderState(RENDERING);
			}
			break;
		case ID_PAUSEITEM:
		case ID_PAUSETOOL:
			if(m_guiRenderState != PAUSED) {
				// Stop display update timer
				m_renderTimer->Stop();
				m_statsTimer->Stop();
				if(m_guiRenderState == RENDERING)
					girlPause();
				ChangeRenderState(PAUSED);
			}
			break;
		case ID_STOPITEM:
		case ID_STOPTOOL:
			if ((m_guiRenderState == RENDERING) || (m_guiRenderState == PAUSED)) {
				// Dade - we can set the enough sample per pixel condition
				m_renderTimer->Stop();
				// Leave stats timer running so we know when threads stopped
				girlSetHaltSamplePerPixel(1, true, true);
				m_statusBar->SetStatusText(wxT("Waiting for render threads to stop."), 0);
				ChangeRenderState(STOPPING);
			}
			break;
		case ID_FULL_SCREEN: // CF
			if ( !IsFullScreen() )
			{
				m_renderToolBar->Show( false );
				m_viewerToolBar->Show( false );
				ShowFullScreen( true );
			}
			else
			{
				ShowFullScreen( false );

				if (m_view->IsChecked( ID_TOOL_BAR ) )
				{
					m_renderToolBar->Show( true );
					m_viewerToolBar->Show( true );
				}
			}
			Layout();
			break;
		case ID_TOOL_BAR: // CF
			if ( m_renderToolBar->IsShown() )
			{
				m_renderToolBar->Show( false );
				m_viewerToolBar->Show( false );
			}
			else
			{
				m_renderToolBar->Show( true );
				m_viewerToolBar->Show( true );
			}
			Layout();
			break;
		case ID_STATUS_BAR: // CF
			if ( m_statusBar->IsShown() )
			{
				m_statusBar->Show( false );
			}
			else
			{
				m_statusBar->Show( true );
			}
			Layout();
			break;
		case ID_RENDER_COPY: // CF
			if ( m_guiRenderState != WAITING &&
				 wxTheClipboard->Open() )
			{
				m_statusBar->SetStatusText(wxT("Copying..."), 0);

				wxTheClipboard->SetData( new wxBitmapDataObject( *(new wxBitmap( *(new wxImage( girlStatistics("filmXres"), girlStatistics("filmYres"), girlFramebuffer()))))));
				wxTheClipboard->Close();

				m_statusBar->SetStatusText(wxT(""), 0);
			}
			break;
		case ID_CLEAR_LOG: // CF
			m_logTextCtrl->Clear();
			break;
		case ID_OPTIONS: // CF
			if ( m_GirlOptions->IsShown() )
			{
				m_GirlOptions->Show( false );
			}
			else
			{
				m_GirlOptions->Show( true );
			}
			break;
		case ID_ADD_THREAD: // CF
			if ( m_numThreads < 16 ) SetRenderThreads( m_numThreads + 1 );
			break;
		case ID_REMOVE_THREAD: // CF
			if ( m_numThreads > 1 ) SetRenderThreads( m_numThreads - 1 );
			break;
		case wxID_ABOUT:
			new wxSplashScreen(m_splashbmp, wxSPLASH_CENTRE_ON_PARENT, 0, this, -1);
			break;
		case wxID_EXIT:
			Close(false);
			break;
		case ID_PAN_MODE: // CF
			m_viewerToolBar->ToggleTool( ID_PANTOOL, true );
			m_renderOutput->SetMode(PANZOOM);
			break;
		case ID_PANTOOL:
			m_renderOutput->SetMode(PANZOOM);
			break;
		case ID_ZOOM_MODE: // CF
			m_viewerToolBar->ToggleTool( ID_ZOOMTOOL, true );
			m_renderOutput->SetMode(SELECTION);
			break;
		case ID_ZOOMTOOL:
		case ID_REFINETOOL:
			m_renderOutput->SetMode(SELECTION);
			break;
		default:
			break;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// CF

void GirlGui::GirlOptions::OnText(wxCommandEvent& event) {
	if ( event.GetEventType() != wxEVT_COMMAND_TEXT_ENTER ) return;

	switch (event.GetId()) {
		case ID_RH_PRESCALE_TEXT:
			if( m_RH_preText->IsModified() )
			{
				wxString st = m_RH_preText->GetValue();

				st.ToDouble( &m_RH_pre );

				if ( m_RH_pre > RH_PRE_RANGE ) m_RH_pre = RH_PRE_RANGE;
				else if ( m_RH_pre < -RH_PRE_RANGE ) m_RH_pre = -RH_PRE_RANGE;

				st = wxString::Format( _("%.02f"), m_RH_pre );

				m_RH_preText->SetValue( st );

				int val = (int)(( 100.0f / (RH_PRE_RANGE * 2.0f) ) * (m_RH_pre + RH_PRE_RANGE)); 

				m_RH_prescaleSlider->SetValue( val );
			}
			break;
		case ID_RH_POSTSCALE_TEXT:
			if ( m_RH_postText->IsModified() )
			{
				wxString st = m_RH_postText->GetValue();

				st.ToDouble( &m_RH_post );

				if ( m_RH_post > RH_POST_RANGE ) m_RH_post = RH_POST_RANGE;
				else if ( m_RH_post < -RH_POST_RANGE ) m_RH_post = -RH_POST_RANGE;

				st = wxString::Format( _("%.02f"), m_RH_post );

				m_RH_postText->SetValue( st );

				int val = (int)(( 100.0f / (RH_POST_RANGE * 2.0f) ) * (m_RH_post + RH_POST_RANGE)); 

				m_RH_postscaleSlider->SetValue( val );
			}
			break;
		case ID_RH_BURN_TEXT:
			if ( m_RH_burnText->IsModified() )
			{
				wxString st = m_RH_burnText->GetValue();

				st.ToDouble( &m_RH_burn );
				if ( m_RH_burn > RH_BURN_RANGE ) m_RH_burn = RH_BURN_RANGE;
				else if ( m_RH_burn < 0 ) m_RH_burn = 0;

				st = wxString::Format( _("%.02f"), m_RH_burn );

				m_RH_burnText->SetValue( st );

				int val = (int)(( 100.0f / RH_BURN_RANGE ) * m_RH_burn); 

				m_RH_burnSlider->SetValue( val );
			}
			break;
		default:
			break;
	}
} 

void GirlGui::GirlOptions::OnScroll( wxScrollEvent& event ){
	switch (event.GetId()) {
		case ID_RH_PRESCALE:
			{
				m_RH_pre = ( (double)event.GetPosition() / ( 100.0f / (RH_PRE_RANGE * 2.0f) ) ) - RH_PRE_RANGE; 

				wxString st = wxString::Format( _("%.02f"), m_RH_pre );
				m_RH_preText->SetValue( st );
			}
			break;
		case ID_RH_POSTSCALE:
			{
				m_RH_post = ( (double)event.GetPosition() / ( 100.0f / (RH_POST_RANGE * 2.0f) ) ) - RH_POST_RANGE; 

				wxString st = wxString::Format( _("%.02f"), m_RH_post );
				m_RH_postText->SetValue( st );
			}
			break;
		case ID_RH_BURN:
			{
				m_RH_burn = (double)event.GetPosition() / ( 100.0f / RH_BURN_RANGE ); 

				wxString st = wxString::Format( _("%.02f"), m_RH_burn );
				m_RH_burnText->SetValue( st );
			}
			break;
		default:
			break;
	}
} 

GirlGui::GirlOptions::GirlOptions( GirlGui *parent ) : m_OptionsDialog( parent ) {

	m_Parent = parent;

	ResetToneMapping();

	wxTextValidator vt( wxFILTER_NUMERIC );

	m_RH_preText->SetValidator( vt );
	m_RH_postText->SetValidator( vt );
	m_RH_burnText->SetValidator( vt );

	UpdateSysOptions();
}

void GirlGui::GirlOptions::UpdateSysOptions( void ) {

	m_DisplayInterval = girlStatistics("displayInterval");
	m_Display_spinCtrl->SetValue( m_DisplayInterval );

	m_WriteInterval = 120;

	m_UseFlm = false;
	m_Write_TGA = false;
	m_Write_TM_EXR = false;
	m_Write_UTM_EXR = false;
	m_Write_TM_IGI = false;
	m_Write_UTM_IGI = false;
}

void GirlGui::GirlOptions::ApplySysOptions( void ){
// TODO
}

void GirlGui::GirlOptions::OnClose( wxCloseEvent& event ){

	Show( false );
	m_Parent->m_view->Check( ID_OPTIONS, false );
}

void GirlGui::GirlOptions::ResetToneMapping(){

	m_RH_pre = 1.0;
	m_RH_post = 1.0;
	m_RH_burn = 6.0;
	
	m_RH_prescaleSlider->SetValue( (int)(( 100.0f / (RH_PRE_RANGE * 2.0f) ) * (m_RH_pre + RH_PRE_RANGE)));
	m_RH_preText->SetValue(_("1.00"));

	m_RH_postscaleSlider->SetValue( (int)(( 100.0f / (RH_POST_RANGE * 2.0f) ) * (m_RH_post + RH_POST_RANGE)));
	m_RH_postText->SetValue(_("1.00"));

	m_RH_burnSlider->SetValue( (int)(( 100.0f / RH_BURN_RANGE ) * m_RH_burn));
	m_RH_burnText->SetValue(_("6.00"));
}

void GirlGui::GirlOptions::OnMenu(wxCommandEvent& event) {
	switch (event.GetId()) {
		case ID_RENDER_REFRESH:
			if ( m_Parent != NULL ) m_Parent->m_renderOutput->Reload();
			break;
		case ID_TM_RESET:
			ResetToneMapping();
			break;
		case ID_SYS_APPLY:
			ApplySysOptions();
			break;
		case ID_WRITE_OPTIONS:
			{
				int nSel = event.GetSelection();
				
				if ( nSel == 0 ) m_UseFlm = event.IsChecked();
				else if ( nSel == 1 ) m_Write_TGA = event.IsChecked();
				else if ( nSel == 2 ) m_Write_TM_EXR = event.IsChecked();
				else if ( nSel == 3 ) m_Write_UTM_EXR = event.IsChecked();
				else if ( nSel == 4 ) m_Write_TM_IGI = event.IsChecked();
				else if ( nSel == 5 ) m_Write_UTM_IGI = event.IsChecked();
			}
			break;
		default:
			break;
	}
}

void GirlGui::GirlOptions::OnSpin( wxSpinEvent& event ) {
	switch (event.GetId()) {
		case ID_SYS_DISPLAY_INT:
			m_DisplayInterval = event.GetPosition();
			break;
		case ID_SYS_WRITE_INT:
			m_WriteInterval = event.GetPosition();
			break;
		default:
			break;
	}
}

// CF
//////////////////////////////////////////////////////////////////////////////////////////////////

void GirlGui::OnOpen(wxCommandEvent& event) {
	if(m_guiRenderState == RENDERING) {
		// Give warning that current rendering is not stoped
		if(wxMessageBox(wxT("Current file is still rendering. Do you want to continue?"), wxT(""), wxYES_NO | wxICON_QUESTION, this) == wxNO)
			return;
	}

	wxFileDialog filedlg(this,
	                     _("Choose a file to open"),
											 wxEmptyString,
											 wxEmptyString,
											 _("Girl scene files (*.girls)|*.girls|All files (*.*)|*.*"),
											 wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if(filedlg.ShowModal() == wxID_OK) {
		m_statusBar->SetStatusText(wxT(""), 0);
		// Clean up if this is not the first rendering
		if(m_guiRenderState != WAITING) {
			if(m_guiRenderState != FINISHED) {
				if(m_updateThread)
					m_updateThread->join();
				girlExit();
				if(m_engineThread)
					m_engineThread->join();
			}
			girlError(GIRL_NOERROR, GIRL_INFO, "Freeing resources.");
			girlCleanup();
			ChangeRenderState(WAITING);
			m_renderOutput->Reset();
		}

		RenderScenefile(filedlg.GetPath());
	}
}

void GirlGui::OnExit(wxCloseEvent& event) {
	//if we have a scene file
  if(m_guiRenderState != WAITING) {
		if(m_updateThread)
			m_updateThread->join();

		girlExit();

		if(m_engineThread)
			m_engineThread->join();

		girlError(GIRL_NOERROR, GIRL_INFO, "Freeing resources.");
		girlCleanup();
	}

	Destroy();
}

void GirlGui::OnError(wxGirlErrorEvent &event) {
	std::stringstream ss("");
	ss << boost::posix_time::second_clock::local_time() << ' ';
	switch(event.GetError()->GetSeverity()) {
		case GIRL_INFO:
			ss << "Info: ";	break;
		case GIRL_WARNING:
			ss << "Warning: ";	break;
		case GIRL_ERROR:
			ss << "Error: ";	break;
		case GIRL_SEVERE:
			ss << "Severe error: ";	break;
	}
	ss << "(" << event.GetError()->GetCode() << ") ";
	ss << event.GetError()->GetMessage() << std::endl;
	m_logTextCtrl->AppendText(wxString::FromAscii(ss.str().c_str()));
	m_logTextCtrl->ShowPosition(m_logTextCtrl->GetLastPosition());
}

void GirlGui::OnTimer(wxTimerEvent& event) {
	switch (event.GetId()) {
		case ID_RENDERUPDATE:
			if(m_updateThread == NULL && girlStatistics("sceneIsReady") &&
			    (m_guiWindowState == SHOWN || m_guiRenderState == FINISHED)) {
				girlError(GIRL_NOERROR, GIRL_INFO, "GUI: Updating framebuffer...");
				m_statusBar->SetStatusText(wxT("Tonemapping..."), 0);
				m_updateThread = new boost::thread(boost::bind(&GirlGui::UpdateThread, this));
			}
			break;
		case ID_STATSUPDATE:
			if(girlStatistics("sceneIsReady")) {
				UpdateStatistics();

				if(m_guiRenderState == STOPPING && m_samplesSec == 0.0) {
					// Render threads stopped, do one last render update
					girlError(GIRL_NOERROR, GIRL_INFO, "GUI: Updating framebuffer...");
					m_statusBar->SetStatusText(wxT("Tonemapping..."), 0);
					m_updateThread = new boost::thread(boost::bind(&GirlGui::UpdateThread, this));
					m_statsTimer->Stop();
					girlPause();
					girlError(GIRL_NOERROR, GIRL_INFO, "Rendering stopped by user.");
					ChangeRenderState(STOPPED);
				}
			}
			break;
		case ID_LOADUPDATE:
			m_progDialog->Pulse();
			if(girlStatistics("sceneIsReady") || m_guiRenderState == FINISHED) {
				m_progDialog->Destroy();
				m_loadTimer->Stop();

				if(girlStatistics("sceneIsReady")) {
					// Scene file loaded
					// Add other render threads if necessary
					int curThreads = 1;
					while(curThreads < m_numThreads) {
						girlAddThread();
						curThreads++;
					}

					// Start updating the display by faking a resume menu item click.
					wxCommandEvent startEvent(wxEVT_COMMAND_MENU_SELECTED, ID_RESUMEITEM);
					GetEventHandler()->AddPendingEvent(startEvent);
				}
			}
			break;
	}
}

void GirlGui::OnCommand(wxCommandEvent &event) {
	if(event.GetEventType() == wxEVT_GIRL_TONEMAPPED) {
		// Make sure the update thread has ended so we can start another one later.
		m_updateThread->join();
		delete m_updateThread;
		m_updateThread = NULL;
		m_statusBar->SetStatusText(wxT(""), 0);
		m_renderOutput->Reload();

	} else if(event.GetEventType() == wxEVT_GIRL_PARSEERROR) {
		wxMessageBox(wxT("Scene file parse error.\nSee log for details."), wxT("Error"), wxOK | wxICON_ERROR, this);
		ChangeRenderState(FINISHED);
	} else if(event.GetEventType() == wxEVT_GIRL_FINISHED && m_guiRenderState == RENDERING) {
		// Ignoring finished events if another file is being opened (state != RENDERING)
		//wxMessageBox(wxT("Rendering is finished."), wxT("GirlRender"), wxOK | wxICON_INFORMATION, this);
		ChangeRenderState(FINISHED);
		// Stop timers and update output one last time.
		m_renderTimer->Stop();
		wxTimerEvent rendUpdEvent(ID_RENDERUPDATE, GetId());
		GetEventHandler()->AddPendingEvent(rendUpdEvent);
		m_statsTimer->Stop();
		wxTimerEvent statUpdEvent(ID_STATSUPDATE, GetId());
		GetEventHandler()->AddPendingEvent(statUpdEvent);
	}
}

void girl::GirlGui::OnIconize( wxIconizeEvent& event )
{
	if(!event.Iconized())
		m_guiWindowState = SHOWN;
	else
		m_guiWindowState = HIDDEN;
}

void GirlGui::RenderScenefile(wxString filename) {
	wxFileName fn(filename);
	SetTitle(wxT("Girl Viewer - ")+fn.GetName());

	// Start main render thread
	m_engineThread = new boost::thread(boost::bind(&GirlGui::EngineThread, this, filename));

	m_progDialog = new wxProgressDialog(wxT("Loading..."), wxT(""), 100, NULL, wxSTAY_ON_TOP);
	m_progDialog->Pulse();
	m_loadTimer->Start(1000, wxTIMER_CONTINUOUS);
}

void GirlGui::OnSelection(wxViewerEvent& event) {
	if(m_viewerToolBar->GetToolState(ID_ZOOMTOOL) == true) {
		// Zoom in and de-select anything
		m_renderOutput->SetZoom(event.GetSelection().get());
		m_renderOutput->SetSelection(NULL);
	} else if(m_viewerToolBar->GetToolState(ID_REFINETOOL) == true) {
		// Highlight current selection
		m_renderOutput->SetHighlight(event.GetSelection().get());
		m_renderOutput->SetSelection(NULL);
		// TODO: Pass selection to the core for actual refinement
	}
}

void GirlGui::EngineThread(wxString filename) {
	boost::filesystem::path fullPath(boost::filesystem::initial_path());
	fullPath = boost::filesystem::system_complete(boost::filesystem::path(filename.fn_str(), boost::filesystem::native));

	chdir(fullPath.branch_path().string().c_str());

	ParseFile(fullPath.leaf().c_str());

	if(!girlStatistics("sceneIsReady")) {
		wxCommandEvent errorEvent(wxEVT_GIRL_PARSEERROR, GetId());
		GetEventHandler()->AddPendingEvent(errorEvent);

		girlWait();
	} else {
		girlWait();

	  girlError(GIRL_NOERROR, GIRL_INFO, "Rendering done.");
		wxCommandEvent endEvent(wxEVT_GIRL_FINISHED, GetId());
		GetEventHandler()->AddPendingEvent(endEvent);
	}
}

void GirlGui::UpdateThread() {
	girlUpdateFramebuffer();
	wxCommandEvent endEvent(wxEVT_GIRL_TONEMAPPED, GetId());
	GetEventHandler()->AddPendingEvent(endEvent);
}

void GirlGui::SetRenderThreads(int num) {
	if(girlStatistics("sceneIsReady")) {
		if(num > m_numThreads) {
			for(; num > m_numThreads; m_numThreads++)
				girlAddThread();
		} else {
			for(; num < m_numThreads; m_numThreads--)
				girlRemoveThread();
		}
	} else {
		m_numThreads = num;
	}

	m_ThreadText->SetLabel( wxString::Format( _("Threads: %02d  "), m_numThreads ) );
}

void GirlGui::UpdateStatistics() {
	m_samplesSec = girlStatistics("samplesSec");
	int samplesSec = Floor2Int(m_samplesSec);
	int samplesTotSec = Floor2Int(girlStatistics("samplesTotSec"));
	int secElapsed = Floor2Int(girlStatistics("secElapsed"));
	double samplesPx = girlStatistics("samplesPx");
	int efficiency = Floor2Int(girlStatistics("efficiency"));

	int secs = (secElapsed) % 60;
	int mins = (secElapsed / 60) % 60;
	int hours = (secElapsed / 3600);

	wxString stats;
	stats.Printf(wxT("%02d:%02d:%02d - %d S/s - %d TotS/s - %.2f S/px - %i%% eff"),
	             hours, mins, secs, samplesSec, samplesTotSec, samplesPx, efficiency);
	m_statusBar->SetStatusText(stats, 1);
}

/*** GirlOutputWin ***/

BEGIN_EVENT_TABLE(GirlOutputWin, wxWindow)
    EVT_PAINT (GirlOutputWin::OnPaint)
END_EVENT_TABLE()

GirlOutputWin::GirlOutputWin(wxWindow *parent)
      : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, -1)), wxViewerBase() {
}

wxWindow* GirlOutputWin::GetWindow() {
	return this;
}

void GirlOutputWin::Reload() {
	Refresh();
}

void GirlOutputWin::OnDraw(wxDC &dc) {
	if (girlStatistics("sceneIsReady")) {
		int w = girlStatistics("filmXres"), h = girlStatistics("filmYres");
		SetVirtualSize(w, h);
		SetScrollRate(1,1);
		unsigned char* fb = girlFramebuffer();
		dc.DrawBitmap(wxBitmap(wxImage(w, h, fb, true)), 0, 0, false);
	}
}

/*** GirlGuiErrorHandler wrapper ***/

void girl::GirlGuiErrorHandler(int code, int severity, const char *msg) {
	// Dade - console print enabled by command line option
	if (copyLog2Console)
		girlErrorPrint(code, severity, msg);

	boost::shared_ptr<GirlError> error(new GirlError(code, severity, msg));
	wxGirlErrorEvent errorEvent(error, wxEVT_GIRL_ERROR);
	wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(errorEvent);
}
