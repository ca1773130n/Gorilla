/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   contribution.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - contribution.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_CONTRIBUTION_H
#define GIRL_CONTRIBUTION_H
// contribution.h*
#include "girl.h"
#include "film.h"
#include "color.h"

#include <boost/thread/recursive_mutex.hpp>

namespace girl
{

// Size of a contribution buffer
// 4096 seems best.
#define CONTRIB_BUF_SIZE 4096

// Minimum number of buffers to keep alive/reuse
// In practice usually up to twice this amount stays allocated
#define CONTRIB_BUF_KEEPALIVE 8

// Switch on to get feedback in the log about allocation
#define CONTRIB_DEBUG false

class GIRLCOREDLL  Contribution {
public:
	Contribution(float x=0.f, float y=0.f, const XYZColor &c=0.f, float a=0.f,
		float v=0.f, int b=0, int g=0) :
		imageX(x), imageY(y), color(c), alpha(a), variance(v),
		buffer(b), bufferGroup(g) { }

	float imageX, imageY;
	XYZColor color;
	float alpha, variance;
	int buffer, bufferGroup;
};

class GIRLCOREDLL  ContributionBuffer {
public:
	ContributionBuffer() : pos(0), sampleCount(0.f) {
		contribs = (Contribution *)AllocAligned(CONTRIB_BUF_SIZE * sizeof(Contribution));
	}

	~ContributionBuffer() {
		FreeAligned(contribs);
	}

	bool Add(Contribution* c, float weight=1.f) {
		if(pos == CONTRIB_BUF_SIZE)
			return false;

		contribs[pos] = *c;
		if(weight != 1.f)
			contribs[pos].color *= weight;

		pos++;

		return true;
	}

	void AddSampleCount(float c, int bufferGroup = 0) { // TODO add support for buffergroup
		sampleCount += c;
	}

	void Splat(Film *film) {
		for (short int i=0; i<CONTRIB_BUF_SIZE; i++)
			film->AddSample(&contribs[i]);
		film->AddSampleCount(sampleCount);
	}

	void Reset() { pos = 0; sampleCount = 0.f; }

private:
	unsigned short int pos;
	float sampleCount;
	Contribution *contribs;
};

class GIRLCOREDLL  ContributionPool {
public:

	ContributionPool() : total(0), splatting(false) { }

	void SetFilm(Film *f) { film = f; }

	ContributionBuffer* Get() { // Do not use Get() directly, use Next() below instead, it's thread-safe.
		u_int freepos = CFree.size();
		if(freepos == 0) {
			ContributionBuffer *cnew = new ContributionBuffer();
			total++;

			if(CONTRIB_DEBUG) {
				std::stringstream ss;
				ss << "Allocating Contribution Buffer - Nr of buffers in pool: "
					<< total << " - Total mem used: " << total * CONTRIB_BUF_SIZE * sizeof(Contribution) << ".";
				girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());
			}

			return cnew;
		}
		ContributionBuffer *cold = CFree[freepos-1];
		CFree.pop_back();
		cold->Reset();
		return cold;
	}

	ContributionBuffer* Next(ContributionBuffer *c) {
		bool doSplat = false;

		boost::recursive_mutex::scoped_lock PoolAction(PoolMutex);

		if(c) CFull.push_back(c);
		ContributionBuffer *cnew = Get();

		if(CFull.size() > CONTRIB_BUF_KEEPALIVE && splatting == false) {
			splatting = doSplat = true;

			for(u_int i=0; i<CFull.size(); i++)
				CSplat.push_back(CFull[i]);

			CFull.clear();
		}

		PoolAction.unlock();

		if(doSplat) {
			for(u_int i=0; i<CSplat.size(); i++) {
				CSplat[i]->Splat(film);
				CSplat[i]->Reset();
			}

			PoolAction.lock();

			for(u_int i=0; i<CSplat.size(); i++)
				CFree.push_back(CSplat[i]);

			CSplat.clear();
			splatting = false;

			PoolAction.unlock();
		}

		return cnew;
	}

	void Flush() {
		for(u_int i=0; i<CSplat.size(); i++) {
			CSplat[i]->Splat(film);
			CSplat[i]->Reset();
		}
	}

	void Delete() {
		for(u_int i=0; i<CFree.size(); i++)
			delete CFree[i];
		for(u_int i=0; i<CFull.size(); i++)
			delete CFull[i];
		for(u_int i=0; i<CSplat.size(); i++)
			delete CSplat[i];
	}

private:
	unsigned int total;
	bool splatting;
	vector<ContributionBuffer*> CFree; // Emptied/available buffers
	vector<ContributionBuffer*> CFull; // Full buffers
	vector<ContributionBuffer*> CSplat; // Buffers being splat

	Film *film;
	boost::recursive_mutex PoolMutex;
};

}//namespace girl

#endif // GIRL_CONTRIBUTION_H
