/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   renderserver.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - renderserver.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef RENDER_SERVER_H
#define RENDER_SERVER_H

#include "girl.h"

#include <fstream>
#include <boost/thread/xtime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/iostreams/filtering_stream.hpp>

namespace girl
{

class GIRLCOREDLL RenderServer;

class GIRLCOREDLL  NetworkRenderServerThread : public boost::noncopyable {
public:
    NetworkRenderServerThread(RenderServer *renderServer) :
        renderServer(renderServer), serverThread(NULL), engineThread(NULL),
        infoThread(NULL), signal(SIG_NONE) { }

    ~NetworkRenderServerThread() {
        if (engineThread)
            delete engineThread;

        if (infoThread)
            delete infoThread;

        if (serverThread)
            delete serverThread;
    }

    void interrupt() {
        signal = SIG_EXIT;
    }

    void join() {
        serverThread->join();
    }

    static void run(NetworkRenderServerThread *serverThread);
    friend class RenderServer;
private:
    RenderServer *renderServer;
    boost::thread *serverThread;
    boost::thread *engineThread;
    boost::thread *infoThread;

    // Dade - used to send signals to the thread
    enum ThreadSignal { SIG_NONE, SIG_EXIT };
    ThreadSignal signal;
    
};

// Dade - network rendering server
class GIRLCOREDLL RenderServer {
public:
    static const int DEFAULT_TCP_PORT = 18018;

    enum ServerState { UNSTARTED, READY, BUSY, STOPPED };

    RenderServer(int threadCount, int tcpPort = DEFAULT_TCP_PORT);
    ~RenderServer();

    void start();
    void join();
    void stop();

    int getServerPort() { return tcpPort; }
    ServerState getServerState() { return  state; }

    friend class NetworkRenderServerThread;

private:
	static string createNewSessionID();

	bool validateAccess(std::basic_istream<char> &stream) const;

    int threadCount;
    int tcpPort;
    ServerState state;
	string currentSID;
    NetworkRenderServerThread *serverThread;
};

}//namespace girl

#endif // RENDER_SERVER_H
