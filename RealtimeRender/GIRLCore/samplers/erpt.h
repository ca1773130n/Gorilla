/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   erpt.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - erpt.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// erpt.h*

#ifndef GIRL_ERPT_H
#define GIRL_ERPT_H

#include "sampling.h"
#include "paramset.h"
#include "film.h"

namespace girl
{

class ERPTSampler : public Sampler {
public:
	ERPTSampler(int totMutations, float microProb, float rng,
		Sampler *sampler);
	~ERPTSampler();

	virtual ERPTSampler* clone() const;
	void GetBufferType(BufferType *type) {*type = BUF_TYPE_PER_SCREEN;}
	u_int GetTotalSamplePos() { return baseSampler->GetTotalSamplePos(); }
	int RoundSize(int size) const { return baseSampler->RoundSize(size); }
	bool GetNextSample(Sample *sample, u_int *use_pos);
	float *GetLazyValues(Sample *sample, u_int num, u_int pos);
	//void AddSample(float imageX, float imageY, const Sample &sample, const Ray &ray, const XYZColor &L, float alpha, int id=0);
	void AddSample(const Sample &sample);
	static Sampler *CreateSampler(const ParamSet &params, const Film *film);

	bool IsMutating() { return true; }

	float LY, baseLY, gain, quantum;
	int normalSamples, totalSamples, totalTimes, totalMutations, chain, numChains, mutation, consecRejects, stamp, numMicro, posMicro;
	float pMicro, range, weight, alpha;
	float *baseImage, *sampleImage, *currentImage;
	int *timeImage, *baseTimeImage, *currentTimeImage, *offset;
	static int initCount, initSamples;
	static float meanIntensity;
	vector<Contribution> oldContributions, baseContributions;
	Sampler *baseSampler;
};

}//namespace girl

#endif // GIRL_METROSAMPLER_H

