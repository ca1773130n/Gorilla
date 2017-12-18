/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   wxgirlapp.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - wxgirlapp.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_WXGIRLAPP_H
#define GIRL_WXGIRLAPP_H

#include "wxgirlgui.h"

namespace girl
{

// The GirlApp class.
class GirlGuiApp : public wxApp {
public:
	virtual bool OnInit();

protected:
	bool ProcessCommandLine();

	GirlGui *m_guiFrame;

	// Command line options
	int m_threads;
	bool m_useServer, m_openglEnabled, m_copyLog2Console;
	wxString m_inputFile;
};

DECLARE_APP(GirlGuiApp)

}//namespace girl

#endif // GIRL_WXGIRLAPP_H
