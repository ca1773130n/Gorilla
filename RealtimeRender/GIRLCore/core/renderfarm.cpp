/***************************************************************************
 *   Copyright (C) 1998-2008 by authors (see AUTHORS.txt )                 *
 *                                                                         *
 *   This file is part of GirlRender.                                       *
 *                                                                         *
 *   Girl Renderer is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Girl Renderer is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *   This project is based on PBRT ; see http://www.pbrt.org               *
 *   Girl Renderer website : http://www.girlrender.net                       *
 ***************************************************************************/

#include "girl.h"
#include "scene.h"
#include "api.h"
#include "error.h"
#include "paramset.h"
#include "renderfarm.h"
#include "camera.h"

#include "../renderer/include/asio.hpp"

#include <fstream>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/bind.hpp>

using namespace boost::iostreams;
using namespace boost::posix_time;
using namespace girl;
using asio::ip::tcp;

void FilmUpdaterThread::updateFilm(FilmUpdaterThread *filmUpdaterThread) {
    // Dade - thread to update the film with data from servers

    boost::xtime reft;
    boost::xtime_get(&reft, boost::TIME_UTC);

    while (filmUpdaterThread->signal == SIG_NONE) {
        // Dade - check signal every 1 sec

        for(;;) {
            // Dade - sleep for 1 sec
            boost::xtime xt;
            boost::xtime_get(&xt, boost::TIME_UTC);
            xt.sec += 1;
            boost::thread::sleep(xt);

            if (filmUpdaterThread->signal == SIG_EXIT)
                break;

            if (xt.sec - reft.sec > filmUpdaterThread->renderFarm->serverUpdateInterval) {
                reft = xt;
                break;
            }
        }

        if (filmUpdaterThread->signal == SIG_EXIT)
            break;

        filmUpdaterThread->renderFarm->updateFilm(filmUpdaterThread->scene);
    }
}

// Dade - used to periodically update the film
void RenderFarm::startFilmUpdater(Scene *scene) {
	if (filmUpdateThread == NULL) {
		filmUpdateThread = new FilmUpdaterThread(this, scene);
		filmUpdateThread->thread = new boost::thread(boost::bind(
			FilmUpdaterThread::updateFilm, filmUpdateThread));
	} else {
		girlError(GIRL_ILLSTATE,GIRL_ERROR,"RenderFarm::startFilmUpdater() called but update thread already started.");
	}
}

void RenderFarm::stopFilmUpdater() {
    if (filmUpdateThread != NULL) {
        filmUpdateThread->interrupt();
        delete filmUpdateThread;
        filmUpdateThread = NULL;
    } else {
		girlError(GIRL_ILLSTATE,GIRL_ERROR,"RenderFarm::stopFilmUpdater() called but update thread not started.");
	}
}

void RenderFarm::decodeServerName(const string &serverName, string &name, string &port) {
	// Dade - check if the server name includes the port
	size_t idx = serverName.find_last_of(':');
	if (idx != string::npos) {
		// Dade - the server name includes the port number

		name = serverName.substr(0, idx);
		port = serverName.substr(idx + 1);
	} else {
		name = serverName;
		port = "18018";
	}
}

bool RenderFarm::connect(const string &serverName) {
    // Dade - connect to the rendering server

    std::stringstream ss;
    try {
        ss.str("");
        ss << "Connecting server: " << serverName;
        girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

        string name, port;
        decodeServerName(serverName, name, port);

        tcp::iostream stream(name, port);
        stream << "ServerConnect" << std::endl;

        // Dede - check if the server accepted the connection

        string result;
        if (!getline(stream, result)) {
            ss.str("");
            ss << "Unable to connect server: " << serverName;
            girlError(GIRL_SYSTEM, GIRL_ERROR, ss.str().c_str());

            return false;
        }

        ss.str("");
        ss << "Server connect result: " << result;
        girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

		string sid;
        if ("OK" != result) {
            ss.str("");
            ss << "Unable to connect server: " << serverName;
            girlError(GIRL_SYSTEM, GIRL_ERROR, ss.str().c_str());

            return false;
		} else {
			// Dade - read the session ID
			if (!getline(stream, result)) {
				ss.str("");
				ss << "Unable read session ID from server: " << serverName;
				girlError(GIRL_SYSTEM, GIRL_ERROR, ss.str().c_str());

				return false;
			}

			sid = result;
			ss.str("");
			ss << "Server session ID: " << sid;
			girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());
		}

		serverInfoList.push_back(ExtRenderingServerInfo(name, port, sid));
    } catch (std::exception& e) {
        ss.str("");
        ss << "Unable to connect server: " << serverName;
        girlError(GIRL_SYSTEM, GIRL_ERROR, ss.str().c_str());

        girlError(GIRL_SYSTEM, GIRL_ERROR, e.what());
        return false;
    }

    flush();

    return true;
}

void RenderFarm::disconnectAll() {
    for (size_t i = 0; i < serverInfoList.size(); i++)
		disconnect(serverInfoList[i]);
	serverInfoList.clear();
}

void RenderFarm::disconnect(const string &serverName) {
	string name, port;
	decodeServerName(serverName, name, port);

	for (vector<ExtRenderingServerInfo>::iterator it = serverInfoList.begin(); it < serverInfoList.end(); it++ ) {
		if (name.compare(it->name) == 0 && port.compare(it->port) == 0) {
			disconnect(*it);
			serverInfoList.erase(it);
			break;
		}
	}
}

void RenderFarm::disconnect(const ExtRenderingServerInfo &serverInfo) {
	std::stringstream ss;
	try {
		ss.str("");
		ss << "Disconnect from server: " <<
				serverInfo.name << ":" << serverInfo.port;
		girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

		tcp::iostream stream(serverInfo.name, serverInfo.port);
		stream << "ServerDisconnect" << std::endl;
		stream << serverInfo.sid << std::endl;
	} catch (std::exception& e) {
		girlError(GIRL_SYSTEM, GIRL_ERROR, e.what());
	}
}

void RenderFarm::flush() {
    std::stringstream ss;
    // Dade - the buffers with all commands
    string commands = netBuffer.str();

    //flush network buffer
	for (size_t i = 0; i < serverInfoList.size(); i++) {
		if(serverInfoList[i].flushed == false) {
			try {
				ss.str("");
				ss << "Sending commands to server: " <<
						serverInfoList[i].name << ":" << serverInfoList[i].port;
				girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

				tcp::iostream stream(serverInfoList[i].name, serverInfoList[i].port);
				stream << commands << std::endl;

				serverInfoList[i].flushed = true;
			} catch (std::exception& e) {
				girlError(GIRL_SYSTEM, GIRL_ERROR, e.what());
			}
		}
    }

    // Dade - write info only if there was the communication with some server
    if (serverInfoList.size() > 0) {
        ss.str("");
        ss << "All servers are aligned";
        girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());
    }
}

void RenderFarm::updateFilm(Scene *scene) {
    // Dade - network rendering supports only FlexImageFilm
    FlexImageFilm *film = (FlexImageFilm *)(scene->camera->film);

    std::stringstream ss;
	for (size_t i = 0; i < serverInfoList.size(); i++) {
        try {
            ss.str("");
            ss << "Getting samples from: " <<
					serverInfoList[i].name << ":" << serverInfoList[i].port;
            girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

            tcp::iostream stream(serverInfoList[i].name, serverInfoList[i].port);
            stream << "girlGetFilm" << std::endl;
			stream << serverInfoList[i].sid << std::endl;

            if (stream.good()) {
                serverInfoList[i].numberOfSamplesReceived += film->UpdateFilm(scene, stream);

                ss.str("");
                ss << "Samples received from '" <<
						serverInfoList[i].name << ":" << serverInfoList[i].port << "'";
                girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

				serverInfoList[i].timeLastContact = second_clock::local_time();
            } else {
                ss.str("");
                ss << "Error while contacting server: " <<
						serverInfoList[i].name << ":" << serverInfoList[i].port;
                girlError(GIRL_SYSTEM, GIRL_ERROR, ss.str().c_str());
            }
        } catch (std::exception& e) {
            girlError(GIRL_SYSTEM, GIRL_ERROR, e.what());
        }
    }
}

void RenderFarm::send(const std::string &command) {
    netBuffer << command << std::endl;
}

void RenderFarm::send(const std::string &command, const std::string &name,
        const ParamSet &params) {
    try {
		netBuffer << command << std::endl << name << std::endl;
        boost::archive::text_oarchive oa(netBuffer);
        oa << params;
    } catch (std::exception& e) {
        girlError(GIRL_SYSTEM, GIRL_ERROR, e.what());
    }
}

void RenderFarm::send(const std::string &command, const std::string &name) {
    try {
        netBuffer << command << std::endl << name << std::endl;
    } catch (std::exception& e) {
        girlError(GIRL_SYSTEM, GIRL_ERROR, e.what());
    }
}

void RenderFarm::send(const std::string &command, float x, float y, float z) {
    try {
        netBuffer << command << std::endl << x << ' ' << y << ' ' << z << std::endl;
    } catch (std::exception& e) {
        girlError(GIRL_SYSTEM, GIRL_ERROR, e.what());
    }
}

void RenderFarm::send(const std::string &command, float a, float x, float y,
        float z) {
    try {
        netBuffer << command << std::endl << a << ' ' << x << ' ' << y << ' ' << z << std::endl;
    } catch (std::exception& e) {
        girlError(GIRL_SYSTEM, GIRL_ERROR, e.what());
    }
}

void RenderFarm::send(const std::string &command, float ex, float ey, float ez,
        float lx, float ly, float lz, float ux, float uy, float uz) {
    try {
        netBuffer << command << std::endl << ex << ' ' << ey << ' ' << ez << ' ' << lx << ' ' << ly << ' ' << lz << ' ' << ux << ' ' << uy << ' ' << uz << std::endl;
    } catch (std::exception& e) {
        girlError(GIRL_SYSTEM, GIRL_ERROR, e.what());
    }
}

void RenderFarm::send(const std::string &command, float tr[16]) {
    try {
        netBuffer << command << std::endl; //<<x<<' '<<y<<' '<<z<<' ';
        for (int i = 0; i < 16; i++)
            netBuffer << tr[i] << ' ';
        netBuffer << std::endl;
    } catch (std::exception& e) {
        girlError(GIRL_SYSTEM, GIRL_ERROR, e.what());
    }
}

void RenderFarm::send(const std::string &command, const string &name,
        const string &type, const string &texname, const ParamSet &params) {
    try {
        netBuffer << command << std::endl << name << std::endl << type << std::endl << texname << std::endl;
        boost::archive::text_oarchive oa(netBuffer);
        oa << params;

        //send the file
        std::string file = "";
        file = params.FindOneString(std::string("filename"), file);
        if (file.size()) {
            std::string s;
            std::ifstream in(file.c_str(), std::ios::out | std::ios::binary);
            while (getline(in, s))
                netBuffer << s << "\n";
            netBuffer << "GIRL_END_FILE\n";
        }

    } catch (std::exception& e) {
        girlError(GIRL_SYSTEM, GIRL_ERROR, e.what());
    }
}

int RenderFarm::getServersStatus(RenderingServerInfo *info, int maxInfoCount) {
	ptime now = second_clock::local_time();
	for (size_t i = 0; i < min<size_t>(serverInfoList.size(), maxInfoCount); ++i) {
		info[i].serverIndex = i;
		info[i].name = serverInfoList[i].name.c_str();
		info[i].port = serverInfoList[i].port.c_str();
		info[i].sid = serverInfoList[i].sid.c_str();

		time_duration td = now - serverInfoList[i].timeLastContact;
		info[i].secsSinceLastContact = td.seconds();
		info[i].numberOfSamplesReceived = serverInfoList[i].numberOfSamplesReceived;
	}

	return serverInfoList.size();
}
