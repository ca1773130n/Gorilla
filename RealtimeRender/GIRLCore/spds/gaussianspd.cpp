/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   gaussianspd.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - gaussianspd.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// gaussianspd.cpp*
#include "gaussianspd.h"

using namespace girl;

void GaussianSPD::init(float mean, float width, float refl) {
	mu = mean;
    wd = width;
	r0 = refl;

	float scale2 = float(-0.5 / (width * width));

	lambdaMin = GAUSS_CACHE_START;
	lambdaMax = GAUSS_CACHE_END;
	delta = (GAUSS_CACHE_END - GAUSS_CACHE_START) / (GAUSS_CACHE_SAMPLES-1);
    invDelta = 1.f / delta;
	nSamples = GAUSS_CACHE_SAMPLES;

	AllocateSamples(GAUSS_CACHE_SAMPLES);

	// Fill samples with Gaussian curve
	for(int i=0; i<GAUSS_CACHE_SAMPLES; i++) {
		float w = (GAUSS_CACHE_START + (delta*i));
		float x = w - mu;
		samples[i] = refl * expf(x * x * scale2);
	}

	Clamp();
}
