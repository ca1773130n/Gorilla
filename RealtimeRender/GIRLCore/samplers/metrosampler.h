/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   metrosampler.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - metrosampler.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// metrosampler.h*

#ifndef GIRL_METROSAMPLER_H
#define GIRL_METROSAMPLER_H

#include "sampling.h"
#include "paramset.h"
#include "film.h"

namespace girl
{

class MetropolisSampler : public Sampler {
public:
	MetropolisSampler(int xStart, int xEnd, int yStart, int yEnd,
			int maxRej, float largeProb, float microProb, float rng, int sw, bool useV);
	~MetropolisSampler();

	virtual MetropolisSampler* clone() const;
	u_int GetTotalSamplePos() { return 0; }
	int RoundSize(int size) const { return size; }
	bool GetNextSample(Sample *sample, u_int *use_pos);
	float *GetLazyValues(Sample *sample, u_int num, u_int pos);
	void AddSample(const Sample &sample);
	static Sampler *CreateSampler(const ParamSet &params, const Film *film);
	void GetBufferType(BufferType *t) { *t = BUF_TYPE_PER_SCREEN; }
	bool IsMutating() { return true; }

	bool large;
	float LY, V;
	int normalSamples, totalSamples, totalTimes, maxRejects, consecRejects, stamp, numMicro, posMicro;
	float pLarge, pMicro, range, weight, alpha;
	float *sampleImage;
	int *timeImage, *offset;
	static int initCount, initSamples;
	static float meanIntensity;
	vector <Contribution> oldContributions;
	float *strataSamples;
	int strataWidth, strataSqr, currentStrata;
	bool useVariance;
};

}//namespace girl

#endif // GIRL_METROSAMPLER_H

