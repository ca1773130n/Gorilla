/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   blackbodyspd.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - blackbodyspd.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// blackbody.cpp*
#include "blackbodyspd.h"

using namespace girl;

void BlackbodySPD::init(float t) {
	temp = t;

	lambdaMin = BB_CACHE_START;
	lambdaMax = BB_CACHE_END;
	delta = (BB_CACHE_END - BB_CACHE_START) / (BB_CACHE_SAMPLES-1);
    invDelta = 1.f / delta;
	nSamples = BB_CACHE_SAMPLES;

	AllocateSamples(BB_CACHE_SAMPLES);

	// Fill samples with BB curve
	for(int i=0; i<BB_CACHE_SAMPLES; i++) {
		float w = 1e-9f * (BB_CACHE_START + (delta*i));
		// Compute blackbody power for wavelength w and temperature temp
		samples[i] = 0.4e-9f * (3.74183e-16f * powf(w, -5.f))
				/ (expf(1.4388e-2f / (w * temp)) - 1.f);
	}

	Normalize();
	Clamp();
}
