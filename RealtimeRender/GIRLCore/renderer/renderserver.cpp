/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   renderserver.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - renderserver.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "include/asio.hpp"
#include "renderserver.h"
#include "api.h"
#include "context.h"
#include "paramset.h"
#include "error.h"

#include "boost/version.hpp"
#if (BOOST_VERSION < 103401)
#include "boost/filesystem/operations.hpp"
#else
#include "boost/filesystem.hpp"
#endif

using namespace girl;
using namespace boost::iostreams;
using namespace boost::filesystem;
using namespace std;
using asio::ip::tcp;

//------------------------------------------------------------------------------
// RenderServer
//------------------------------------------------------------------------------

RenderServer::RenderServer(int threadCount, int tcpPort) : threadCount(threadCount),
        tcpPort(tcpPort), state(UNSTARTED), serverThread(NULL) {
}

RenderServer::~RenderServer() {
    if ((state == READY) && (state == BUSY))
        stop();
}

void RenderServer::start() {
    if (state != UNSTARTED) {
        stringstream ss;
        ss << "Can not start a rendering server in state: " << state;
        girlErrorPrint(GIRL_SYSTEM, GIRL_ERROR, ss.str().c_str());

        return;
    }

    // Dade - start the tcp server thread
    serverThread = new NetworkRenderServerThread(this);
    serverThread->serverThread = new boost::thread(boost::bind(
            NetworkRenderServerThread::run, serverThread));

    state = READY;
}

void RenderServer::join() {
    if ((state != READY) && (state != BUSY)) {
        stringstream ss;
        ss << "Can not join a rendering server in state: " << state;
        girlErrorPrint(GIRL_SYSTEM, GIRL_ERROR, ss.str().c_str());

        return;
    }

    serverThread->join();
}

void RenderServer::stop() {
    if ((state != READY) && (state != BUSY)) {
        stringstream ss;
        ss << "Can not stop a rendering server in state: " << state;
        girlErrorPrint(GIRL_SYSTEM, GIRL_ERROR, ss.str().c_str());

        return;
    }

    serverThread->interrupt();
    serverThread->join();

    state = STOPPED;
}

//------------------------------------------------------------------------------
// NetworkRenderServerThread
//------------------------------------------------------------------------------

static void printInfoThread() {
    while (true) {
        boost::xtime xt;
        boost::xtime_get(&xt, boost::TIME_UTC);
        xt.sec += 5;
        boost::thread::sleep(xt);

        boost::posix_time::time_duration td(0, 0,
                (int) girlStatistics("secElapsed"), 0);

        int sampleSec = (int)girlStatistics("samplesSec");
        // Dade - print only if we are rendering something
        if (sampleSec > 0) {
            stringstream ss;
            ss << td << "  " << sampleSec << " samples/sec " << " "
                << (float) girlStatistics("samplesPx") << " samples/pix";
            girlErrorPrint(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());
        }
    }
}

static void processCommandFilm(void (&f)(const string &, const ParamSet &), basic_istream<char> &stream) {
    string type;
	getline(stream, type);

    if((type != "fleximage") && (type != "multiimage")) {
        stringstream ss;
        ss << "Unsupported film type for server rendering: " << type;
        girlErrorPrint(GIRL_SYSTEM, GIRL_ERROR, ss.str().c_str());

        return;
    }

	boost::archive::text_iarchive ia(stream);
	ParamSet params;
    ia >> params;

    // Dade - overwrite some option for the servers

    params.EraseBool("write_tonemapped_exr");
    params.EraseBool("write_untonemapped_exr");
    params.EraseBool("write_untonemapped_igi");
	params.EraseBool("write_tonemapped_igi");
    params.EraseBool("write_tonemapped_tga");
    params.EraseBool("write_resume_flm");

	bool no = false;
    params.AddBool("write_tonemapped_exr", &no);
    params.AddBool("write_untonemapped_exr", &no);
    params.AddBool("write_untonemapped_igi", &no);
	params.AddBool("write_tonemapped_igi", &no);
    params.AddBool("write_tonemapped_tga", &no);
    params.AddBool("write_resume_flm", &no);

    f(type.c_str(), params);
}

static void processCommand(void (&f)(const string &, const ParamSet &), basic_istream<char> &stream) {
    string type;
	getline(stream, type);
	
	ParamSet params;
    boost::archive::text_iarchive ia(stream);
    ia >> params;

    f(type.c_str(), params);
}

static void processCommand(void (&f)(const string &), basic_istream<char> &stream) {
    string type;
	getline(stream, type);

	f(type.c_str());
}

static void processCommand(void (&f)(float, float, float), basic_istream<char> &stream) {
    float ax, ay, az;
    stream >> ax;
    stream >> ay;
    stream >> az;
    f(ax, ay, az);
}

static void processCommand(void (&f)(float[16]), basic_istream<char> &stream) {
    float t[16];
    for (int i = 0; i < 16; i++)
        stream >> t[i];
    f(t);
}

string RenderServer::createNewSessionID() {
	char buf[4 * 4 + 4];
	sprintf(buf, "%04d_%04d_%04d_%04d", rand() % 9999, rand() % 9999,
			rand() % 9999, rand() % 9999);

	return string(buf);
}

bool RenderServer::validateAccess(basic_istream<char> &stream) const {
	if (serverThread->renderServer->state != RenderServer::BUSY)
		return false;

	string sid;
	if (!getline(stream, sid))
		return false;

	stringstream ss;
    ss << "Validating SID: " << sid << " = " << currentSID;
    girlErrorPrint(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

	return (sid == currentSID);
}

// Dade - TODO: support signals
void NetworkRenderServerThread::run(NetworkRenderServerThread *serverThread) {
    //Command identifiers (hashed strings)
    //These identifiers are used in the switch statement to avoid string comparisons (fastest)
    static const unsigned int CMD_GIRLINIT = 2531407474U, CMD_GIRLTRANSLATE = 2039302476U, CMD_GIRLROTATE = 3982485453U, CMD_GIRLSCALE = 1943523046U,
            CMD_GIRLLOOKAT = 3747502632U, CMD_GIRLCONCATTRANSFORM = 449663410U, CMD_GIRLTRANSFORM = 2039102042U, CMD_GIRLIDENTITY = 97907816U,
            CMD_GIRLCOORDINATESYSTEM = 1707244427U, CMD_GIRLCOORDSYSTRANSFORM = 2024449520U, CMD_GIRLPIXELFILTER = 2384561510U,
            CMD_GIRLFILM = 2531294310U, CMD_GIRLSAMPLER = 3308802546U, CMD_GIRLACCELERATOR = 1613429731U, CMD_GIRLSURFACEINTEGRATOR = 4011931910U,
            CMD_GIRLVOLUMEINTEGRATOR = 2954204437U, CMD_GIRLCAMERA = 3378604391U, CMD_GIRLWORLDBEGIN = 1247285547U,
            CMD_GIRLATTRIBUTEBEGIN = 684297207U, CMD_GIRLATTRIBUTEEND = 3427929065U, CMD_GIRLTRANSFORMBEGIN = 567425599U,
            CMD_GIRLTRANSFORMEND = 2773125169U, CMD_GIRLTEXTURE = 475043887U, CMD_GIRLMATERIAL = 4064803661U,
            CMD_GIRLMAKENAMEDMATERIAL = 2355625968U, CMD_GIRLNAMEDMATERIAL = 922480690U, CMD_GIRLLIGHTSOURCE = 130489799U,
            CMD_GIRLAREALIGHTSOURCE = 515057184U, CMD_GIRLPORTALSHAPE = 3416559329U, CMD_GIRLSHAPE = 1943702863U,
            CMD_GIRLREVERSEORIENTATION = 2027239206U, CMD_GIRLVOLUME = 4138761078U, CMD_GIRLOBJECTBEGIN = 1097337658U,
            CMD_GIRLOBJECTEND = 229760620U, CMD_GIRLOBJECTINSTANCE = 4125664042U, CMD_GIRLWORLDEND = 1582674973U, CMD_GIRLGETFILM = 859419430U,
            CMD_SERVER_DISCONNECT = 2500584742U, CMD_SERVER_CONNECT = 332355398U,
            CMD_VOID = 5381U, CMD_SPACE = 177605U;

    int listenPort = serverThread->renderServer->tcpPort;
    stringstream ss;
    ss << "Launching server [" << serverThread->renderServer->threadCount <<
            " threads] mode on port '" << listenPort << "'.";
    girlErrorPrint(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

	vector<string> tmpFileList;
    try {
        asio::io_service io_service;
        tcp::endpoint endpoint(tcp::v4(), listenPort);
        tcp::acceptor acceptor(io_service, endpoint);

        for (;;) {
            tcp::iostream stream;
            acceptor.accept(*stream.rdbuf());
            stream.setf(ios::scientific, ios::floatfield);
            stream.precision(16);

            //reading the command
            string command;
            while (getline(stream, command)) {
                unsigned int hash = DJBHash(command);

                if ((command != "") && (command != " ")) {
                    ss.str("");
                    ss << "Server processing command: '" << command <<
                            "' (hash: " << hash << ")";
                    girlErrorPrint(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());
                }

                //processing the command
                switch (hash) {
                    case CMD_VOID:
                    case CMD_SPACE:
                        break;
                    case CMD_SERVER_DISCONNECT:
						if (!serverThread->renderServer->validateAccess(stream))
							break;

                        // Dade - stop the rendering and cleanup
                        girlExit();
                        girlWait();
                        girlCleanup();

						// Dade - remove all temporary files
						for (size_t i = 0; i < tmpFileList.size(); i++)
							remove(tmpFileList[i]);

                        serverThread->renderServer->state = RenderServer::READY;
                        break;
                    case CMD_SERVER_CONNECT:
                        if (serverThread->renderServer->state == RenderServer::READY) {
                            serverThread->renderServer->state = RenderServer::BUSY;
                            stream << "OK" << endl;

							// Dade - generate the session ID
							serverThread->renderServer->currentSID =
									RenderServer::createNewSessionID();
							ss.str("");
							ss << "New session ID: " << serverThread->renderServer->currentSID;
							girlErrorPrint(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());
							stream << serverThread->renderServer->currentSID << endl;

							tmpFileList.clear();
                        } else
                            stream << "BUSY" << endl;
                        break;
                    case CMD_GIRLINIT:
                        girlErrorPrint(GIRL_BUG, GIRL_SEVERE, "Server already initialized");
                        break;
                    case CMD_GIRLTRANSLATE: processCommand(Context::girlTranslate, stream);
                        break;
                    case CMD_GIRLROTATE:
                    {
                        float angle, ax, ay, az;
                        stream >> angle;
                        stream >> ax;
                        stream >> ay;
                        stream >> az;
                        //cout<<"params :"<<angle<<", "<<ax<<", "<<ay<<", "<<az<< endl;
                        girlRotate(angle, ax, ay, az);
                    }
                        break;
                    case CMD_GIRLSCALE: processCommand(Context::girlScale, stream);
                        break;
                    case CMD_GIRLLOOKAT:
                    {
                        float ex, ey, ez, lx, ly, lz, ux, uy, uz;
                        stream >> ex;
                        stream >> ey;
                        stream >> ez;
                        stream >> lx;
                        stream >> ly;
                        stream >> lz;
                        stream >> ux;
                        stream >> uy;
                        stream >> uz;
                        //cout<<"params :"<<ex<<", "<<ey<<", "<<ez<<", "<<lx<<", "<<ly<<", "<<lz<<", "<<ux<<", "<<uy<<", "<<uz<< endl;
                        girlLookAt(ex, ey, ez, lx, ly, lz, ux, uy, uz);
                    }
                        break;
                    case CMD_GIRLCONCATTRANSFORM: processCommand(Context::girlConcatTransform, stream);
                        break;
                    case CMD_GIRLTRANSFORM: processCommand(Context::girlTransform, stream);
                        break;
                    case CMD_GIRLIDENTITY: Context::girlIdentity();
                        break;
                    case CMD_GIRLCOORDINATESYSTEM: processCommand(Context::girlCoordinateSystem, stream);
                        break;
                    case CMD_GIRLCOORDSYSTRANSFORM: processCommand(Context::girlCoordSysTransform, stream);
                        break;
                    case CMD_GIRLPIXELFILTER: processCommand(Context::girlPixelFilter, stream);
                        break;
                    case CMD_GIRLFILM:
                    {
                        // Dade - Servers use a special kind of film to buffer the
                        // samples. I overwrite some option here.

                        processCommandFilm(Context::girlFilm, stream);
                    }
                        break;
                    case CMD_GIRLSAMPLER: processCommand(Context::girlSampler, stream);
                        break;
                    case CMD_GIRLACCELERATOR: processCommand(Context::girlAccelerator, stream);
                        break;
                    case CMD_GIRLSURFACEINTEGRATOR: processCommand(Context::girlSurfaceIntegrator, stream);
                        break;
                    case CMD_GIRLVOLUMEINTEGRATOR: processCommand(Context::girlVolumeIntegrator, stream);
                        break;
                    case CMD_GIRLCAMERA: processCommand(Context::girlCamera, stream);
                        break;
                    case CMD_GIRLWORLDBEGIN: Context::girlWorldBegin();
                        break;
                    case CMD_GIRLATTRIBUTEBEGIN: Context::girlAttributeBegin();
                        break;
                    case CMD_GIRLATTRIBUTEEND: Context::girlAttributeEnd();
                        break;
                    case CMD_GIRLTRANSFORMBEGIN: Context::girlTransformBegin();
                        break;
                    case CMD_GIRLTRANSFORMEND: Context::girlTransformEnd();
                        break;
                    case CMD_GIRLTEXTURE:
                    {
                        string name, type, texname;
                        ParamSet params;
                        stream >> name;
                        stream >> type;
                        stream >> texname;
                        boost::archive::text_iarchive ia(stream);
                        ia >> params;

                        string originalFile = params.FindOneString(string("filename"), "");
                        if (originalFile.size()) {
							// Dade - look for file extension
							string fileExt = "";
							size_t idx = originalFile.find_last_of('.');
							if (idx != string::npos)
								fileExt = originalFile.substr(idx);

							// Dade - replace the file name with a temporary name
							char buf[64];
							sprintf(buf,"%05d_%08d%s", serverThread->renderServer->tcpPort,
									tmpFileList.size(), fileExt.c_str());
							string file = string(buf);

							// Dade - replace the filename parameter
							params.AddString("filename", &file);

							ss.str("");
							ss << "Receiving file: '" << originalFile << "' (in '" <<
									file << "')";
							girlErrorPrint(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

                            bool first = true;
                            string s;
                            ofstream out(file.c_str(), ios::out | ios::binary);
                            while (getline(stream, s) && (s != "GIRL_END_FILE")) {
                                if (!first)
									out << "\n";
                                first = false;
                                out << s;
                            }
                            out.flush();

							tmpFileList.push_back(file);
                        }

                        Context::girlTexture(name.c_str(), type.c_str(), texname.c_str(), params);
                    }
                        break;
                    case CMD_GIRLMATERIAL: processCommand(Context::girlMaterial, stream);
                        break;
                    case CMD_GIRLMAKENAMEDMATERIAL: processCommand(Context::girlMakeNamedMaterial, stream);
                        break;
                    case CMD_GIRLNAMEDMATERIAL: processCommand(Context::girlNamedMaterial, stream);
                        break;
                    case CMD_GIRLLIGHTSOURCE: processCommand(Context::girlLightSource, stream);
                        break;
                    case CMD_GIRLAREALIGHTSOURCE: processCommand(Context::girlAreaLightSource, stream);
                        break;
                    case CMD_GIRLPORTALSHAPE: processCommand(Context::girlPortalShape, stream);
                        break;
                    case CMD_GIRLSHAPE: processCommand(Context::girlShape, stream);
                        break;
                    case CMD_GIRLREVERSEORIENTATION: Context::girlReverseOrientation();
                        break;
                    case CMD_GIRLVOLUME: processCommand(Context::girlVolume, stream);
                        break;
                    case CMD_GIRLOBJECTBEGIN: processCommand(Context::girlObjectBegin, stream);
                        break;
                    case CMD_GIRLOBJECTEND: Context::girlObjectEnd();
                        break;
                    case CMD_GIRLOBJECTINSTANCE: processCommand(Context::girlObjectInstance, stream);
                        break;
                    case CMD_GIRLWORLDEND:
                    {
                        serverThread->engineThread = new boost::thread(&girlWorldEnd);

                        // Wait the scene parsing to finish
                        while (!girlStatistics("sceneIsReady")) {
                            boost::xtime xt;
                            boost::xtime_get(&xt, boost::TIME_UTC);
                            xt.sec += 1;
                            boost::thread::sleep(xt);
                        }

                        // Dade - start the info thread only if it is not already running
                        if(!serverThread->infoThread)
                            serverThread->infoThread = new boost::thread(&printInfoThread);

                        // Add rendering threads
                        int threadsToAdd = serverThread->renderServer->threadCount;
                        while (--threadsToAdd)
                            Context::girlAddThread();
                    }
                        break;
                    case CMD_GIRLGETFILM:
                    {
						if (!serverThread->renderServer->validateAccess(stream))
							break;

                        // Dade - check if we are rendering something
                        if (serverThread->renderServer->state == RenderServer::BUSY) {
                            girlErrorPrint(GIRL_NOERROR, GIRL_INFO, "Transmitting film samples");

                            Context::girlTransmitFilm(stream);
                            stream.close();

                            girlErrorPrint(GIRL_NOERROR, GIRL_INFO, "Finished film samples transmission");
                        } else {
                            girlErrorPrint(GIRL_SYSTEM, GIRL_ERROR, "Received a GetFilm command after a ServerDisconnect");
                            stream.close();
                        }
                    }
                        break;
                    default:
                        ss.str("");
                        ss << "Unknown command '" << command << "'. Ignoring";
                        girlErrorPrint(GIRL_BUG, GIRL_SEVERE, ss.str().c_str());
                        break;
                }

                //END OF COMMAND PROCESSING
            }
        }
    } catch (exception& e) {
		ss.str("");
		ss << "Internal error: " << e.what();
        girlErrorPrint(GIRL_BUG, GIRL_ERROR, ss.str().c_str());
    }
}
