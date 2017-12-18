/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   wxgirlapp.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - wxgirlapp.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// For compilers that don't support precompilation, include "wx/wx.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#	include "wx/wx.h"
#endif

#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#include "girl.h"
#include "api.h"
#include "error.h"
#include "osfunc.h"

#include "wxgirlapp.h"
#include "wxgirlgui.h"

using namespace girl;
namespace po = boost::program_options;

IMPLEMENT_APP(GirlGuiApp)

// This is executed upon startup, like 'main()' in non-wxWidgets programs.
bool GirlGuiApp::OnInit() {
	// Dade - initialize rand() number generator
	srand(time(NULL));

	// Set numeric format to standard to avoid errors when parsing files
	setlocale(LC_NUMERIC, "C");

	girlInit();

	if(ProcessCommandLine()) {
		m_guiFrame = new GirlGui((wxFrame*)NULL, m_openglEnabled, m_copyLog2Console);
		m_guiFrame->Show(true);
		SetTopWindow(m_guiFrame);
		m_guiFrame->SetRenderThreads(m_threads);
		if(!m_inputFile.IsEmpty())
			m_guiFrame->RenderScenefile(m_inputFile);
		return true;
	} else {
		return false;
	}
}

bool GirlGuiApp::ProcessCommandLine() {
	try {
		// allowed only on command line
		po::options_description generic("Generic options");
		generic.add_options()
			("version,v", "Print version string")
			("help", "Produce help message")
			("debug,d", "Enable debug mode")
		;

		// Declare a group of options that will be
		// allowed both on command line and in
		// config file
		po::options_description config("Configuration");
		config.add_options()
			("threads,t", po::value < int >(), "Specify the number of threads that Girl will run in parallel.")
			("useserver,u", po::value< std::vector<std::string> >()->composing(), "Specify the adress of a rendering server to use.")
			("serverinterval,i", po::value < int >(), "Specify the number of seconds between requests to rendering servers.")
			("logconsole,l", "Copy the output of the log tab to the console.")
		;

		// Hidden options, will be allowed both on command line and
		// in config file, but will not be shown to the user.
		po::options_description hidden("Hidden options");
		hidden.add_options()
			("input-file", po::value < vector < string > >(), "input file")
		;

		#ifdef GIRL_USE_OPENGL
			generic.add_options()
				("noopengl", "Disable OpenGL to display the image")
			;
		#else
			hidden.add_options()
				("noopengl", "Disable OpenGL to display the image")
			;
		#endif // GIRL_USE_OPENGL

		po::options_description cmdline_options;
		cmdline_options.add(generic).add(config).add(hidden);

		po::options_description config_file_options;
		config_file_options.add(config).add(hidden);

		po::options_description visible("Allowed options");
		visible.add(generic).add(config);

		po::positional_options_description p;

		p.add("input-file", -1);

		po::variables_map vm;
	#if wxUSE_UNICODE == 1
		store(po::wcommand_line_parser(wxApp::argc, wxApp::argv).
			options(cmdline_options).positional(p).run(), vm);
	#else // ANSI
		store(po::command_line_parser(wxApp::argc, wxApp::argv).
			options(cmdline_options).positional(p).run(), vm);
	#endif // Unicode/ANSI

		std::ifstream ifs("girlrender.cfg");
		store(parse_config_file(ifs, config_file_options), vm);
		notify(vm);

		if(vm.count("help")) {
			std::cout << "Usage: girlrender [options] file..." << std::endl;
			std::cout << visible << std::endl;
			return false;
		}

		if(vm.count("version")) {
			std::cout << "Girl version " << GIRL_VERSION_STRING << " of " << __DATE__ << " at " << __TIME__ << std::endl;
			return false;
		}

		if(vm.count("logconsole"))
			m_copyLog2Console = true;
		else
			m_copyLog2Console = false;

		if(vm.count("threads")) {
			m_threads = vm["threads"].as < int >();
		} else {
			// Dade - check for the hardware concurrency available
			m_threads = osHardwareConcurrency();
			if (m_threads == 0)
				m_threads = 1;
		}

		if(vm.count("debug")) {
			girlError(GIRL_NOERROR, GIRL_INFO, "Debug mode enabled");
			girlEnableDebugMode();
		}

		int serverInterval;
		if(vm.count("serverinterval")) {
			serverInterval = vm["serverinterval"].as<int>();
			girlSetNetworkServerUpdateInterval(serverInterval);
		} else {
			serverInterval = girlGetNetworkServerUpdateInterval();
		}

		if(vm.count("useserver")) {
			std::stringstream ss;

			std::vector<std::string> names = vm["useserver"].as<std::vector<std::string> >();

			for(std::vector<std::string>::iterator i = names.begin(); i < names.end(); i++) {
				ss.str("");
				ss << "Connecting to server '" <<(*i) << "'";
				girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

				//TODO jromang : try to connect to the server, and get version number. display message to see if it was successfull
				girlAddServer((*i).c_str());
			}

			m_useServer = true;

			ss.str("");
			ss << "Server requests interval:  " << serverInterval << " secs";
			girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());
		} else {
			m_useServer = false;
		}

		if(vm.count("noopengl")) {
			m_openglEnabled = false;
		} else {
			#ifdef GIRL_USE_OPENGL
				#if wxUSE_GLCANVAS == 1
					m_openglEnabled = true;
				#else
					m_openglEnabled = false;
					girlError(GIRL_NOERROR, GIRL_INFO, "GUI: wxWidgets without suppport for OpenGL canvas - will not be used.");
				#endif // wxUSE_GLCANVAS
			#else
				m_openglEnabled = false;
				girlError(GIRL_NOERROR, GIRL_INFO, "GUI: OpenGL support was not compiled in - will not be used.");
			#endif // GIRL_USE_OPENGL
		}

		if(vm.count("input-file")) {
			const std::vector < std::string > &v = vm["input-file"].as < vector < string > >();
			if(v.size() > 1) {
				girlError(GIRL_SYSTEM, GIRL_SEVERE, "More than one file passed on command line : rendering the first one.");
			}

			m_inputFile = wxString(v[0].c_str(), wxConvUTF8);
		} else {
			m_inputFile.Clear();
		}

		return true;
	} catch(std::exception &e) {
		std::cout << e.what() << std::endl;
		return false;
	}
}
