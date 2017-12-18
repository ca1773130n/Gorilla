#include "stdafx.h"
#include "GGIRLLoader.h"

using namespace girl;
namespace po = boost::program_options;

int		threads;
bool	parseError;
string	sceneFileName;

VOID
engineThread()
{
	ParseFile(sceneFileName.c_str());
	if (girlStatistics("sceneIsReady") == 0.)
		parseError = true;
}

VOID
infoThread()
{
	while (true) {
		boost::xtime xt;
		boost::xtime_get(&xt, boost::TIME_UTC);
		xt.sec += 60;
		//xt.sec += 30;
		boost::thread::sleep(xt);

		boost::posix_time::time_duration td(0, 0,
			(int) girlStatistics("secElapsed"), 0);

		int sampleSec = (int)girlStatistics("samplesSec");

		if (sampleSec > 0) {
			std::stringstream ss;
			ss << '[' << threads << " threads] " << td << " "
				<< sampleSec << " samples/sec " << " "
				<< (int) girlStatistics("samplesTotSec") << " samples/totsec " << " "
				<< (float) girlStatistics("samplesPx") << " samples/pix";
			//girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());
			girlProgressMsgPrint(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());
		}
	}
}

VOID CGGIRLLoader::Load( char* szFileName )
{
	// Dade - initialize rand() number generator
	srand(time(NULL));
	bool useServer = false;
	girlInit();

	try {
		
		std::stringstream ss;
		/*
		// Declare a group of options that will be
		// allowed only on command line
		po::options_description generic("Generic options");
		generic.add_options()
			("version,v", "Print version string") ("help", "Produce help message")
			("server,s", "Launch in server mode")
			("bindump,b", "Dump binary RGB framebuffer to stdout when finished")
			("debug,d", "Enable debug mode")
			;

		// Declare a group of options that will be
		// allowed both on command line and in
		// config file
		po::options_description config("Configuration");
		config.add_options()
			("threads,t", po::value < int >(), "Specify the number of threads that GRealtime will run in parallel.")
			("useserver,u", po::value< std::vector<std::string> >()->composing(), "Specify the adress of a rendering server to use.")
			("serverinterval,i", po::value < int >(), "Specify the number of seconds between requests to rendering servers.")
			("serverport,p", po::value < int >(), "Specify the tcp port used in server mode.")
			;

		// Hidden options, will be allowed both on command line and
		// in config file, but will not be shown to the user.
		po::options_description hidden("Hidden options");
		hidden.add_options()
			("input-file", po::value< vector<string> >(), "input file")
			("test", "debug test mode")
			;

		po::options_description cmdline_options;
		cmdline_options.add(generic).add(config).add(hidden);

		po::options_description config_file_options;
		config_file_options.add(config).add(hidden);

		po::options_description visible("Allowed options");
		visible.add(generic).add(config);

		po::positional_options_description p;

		p.add("input-file", -1);

		po::variables_map vm;
		store(po::command_line_parser(ac, av).options(cmdline_options).positional(p).run(), vm);

		std::ifstream ifs("girlconsole.cfg");
		store(parse_config_file(ifs, config_file_options), vm);
		notify(vm);

		if (vm.count("help")) {
			ss.str("");
			ss << "Usage: girlconsole [options] file...\n" << visible;
			girlErrorPrint(GIRL_SYSTEM, GIRL_ERROR, ss.str().c_str());
			return 0;
		}

		ss.str("");
		ss << "GRealtimeConsole Ver. 2.1.0 \n";
		girlProgressMsgPrint(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

		if (vm.count("version"))
			return 0;

		if (vm.count("threads"))
			threads = vm["threads"].as<int>();
		else {
			// Dade - check for the hardware concurrency available
			threads = osHardwareConcurrency();
			if (threads == 0)
				threads = 1;
		}

		ss.str("");
		ss << "Threads: " << threads << "\n";
		girlProgressMsgPrint(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

		if (vm.count("debug")) {
			girlErrorPrint(GIRL_NOERROR, GIRL_INFO, "Debug mode enabled");
			girlEnableDebugMode();
		}

		int serverInterval;
		if (vm.count("serverinterval")) {
			serverInterval = vm["serverinterval"].as<int>();
			girlSetNetworkServerUpdateInterval(serverInterval);
		} else
			serverInterval = girlGetNetworkServerUpdateInterval();

		if (vm.count("useserver")) {
			std::vector<std::string> names = vm["useserver"].as<std::vector<std::string> >();

			for (std::vector<std::string>::iterator i = names.begin(); i < names.end(); i++)
				girlAddServer((*i).c_str());

			useServer = true;

			ss.str("");
			ss << "Server requests interval: " << serverInterval << " secs";
			girlErrorPrint(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());
		}

		int serverPort = RenderServer::DEFAULT_TCP_PORT;
		if (vm.count("serverport"))
			serverPort = vm["serverport"].as<int>();
		*/

		//if (vm.count("input-file")) {
			//const std::vector<std::string> &v = vm["input-file"].as < vector<string> > ();
			//for (unsigned int i = 0; i < v.size(); i++) {
				//change the working directory
				boost::filesystem::path fullPath(boost::filesystem::initial_path());
				std::string strFileName(szFileName);
				fullPath = boost::filesystem::system_complete(boost::filesystem::path(strFileName, boost::filesystem::native));

				if (!boost::filesystem::exists(fullPath) && strFileName != "-") {
					ss.str("");
					ss << "Unable to open scenefile '" << fullPath.string() << "'";
					girlErrorPrint(GIRL_NOFILE, GIRL_SEVERE, ss.str().c_str());
					return;
				}

				sceneFileName = fullPath.leaf();
				if (chdir(fullPath.branch_path().string().c_str())) {
					ss.str("");
					ss << "Unable to go into directory '" << fullPath.branch_path().string() << "'";
					girlErrorPrint(GIRL_NOFILE, GIRL_SEVERE, ss.str().c_str());
				}

				//  [12/4/2008 Sung-Soo Kim] Reading GIRL scene file.
				ss.str("");
				ss << "Reading " << sceneFileName.c_str() << "\n";
				girlProgressMsgPrint(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

				parseError = false;
				//boost::thread engine(&engineThread);
				engineThread();

				ss.str("");
				ss << "GIRL scene file read successfully " << "\n";
				girlProgressMsgPrint(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

				//wait the scene parsing to finish
				/*
				while (!girlStatistics("sceneIsReady") && !parseError) {
					boost::xtime xt;
					boost::xtime_get(&xt, boost::TIME_UTC);
					xt.sec += 1;
					boost::thread::sleep(xt);
				}
				*/

				if (parseError) {
					std::stringstream ss;
					ss << "Skipping invalid scenefile '" << fullPath.string() << "'";
					girlErrorPrint(GIRL_BADFILE, GIRL_SEVERE, ss.str().c_str());
					return;
				}

				//add rendering threads
				/*
				int threadsToAdd = threads;
				while (--threadsToAdd)
					Context::girlAddThread();

				//launch info printing thread
				boost::thread info(&infoThread);

				// Dade - wait for the end of the rendering
				girlWait();
				girlExit();

				// Dade - print the total rendering time
				boost::posix_time::time_duration td(0, 0,
					(int) girlStatistics("secElapsed"), 0);

				ss.str("");
				ss << "100% rendering done [" << threads << " threads] " << td;
				girlProgressMsgPrint(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

				girlCleanup();
				*/
			//}
		//} 
		/*
		else if (vm.count("server")) {
			RenderServer *renderServer = new RenderServer(threads, serverPort);
			renderServer->start();
			renderServer->join();
			delete renderServer;
		} else {
			ss.str("");
			ss << "girlconsole: no input file";
			girlErrorPrint(GIRL_SYSTEM, GIRL_ERROR, ss.str().c_str());
		}
		*/
	} catch (std::exception & e) {
		std::stringstream ss;
		ss << "Command line argument parsing failed with error '" << e.what() << "', please use the --help option to view the allowed syntax.";
		girlErrorPrint(GIRL_SYNTAX, GIRL_SEVERE, ss.str().c_str());
	}
}