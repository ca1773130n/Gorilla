/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   frequencyspd.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - frequencyspd.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// frequencyspd.cpp*
#include "frequencyspd.h"

using namespace girl;

void FrequencySPD::init(float freq, float phase, float refl) {
	fq = freq;
    ph = phase;
	r0 = refl;

	lambdaMin = FREQ_CACHE_START;
	lambdaMax = FREQ_CACHE_END;
	delta = (FREQ_CACHE_END - FREQ_CACHE_START) / (FREQ_CACHE_SAMPLES-1);
    invDelta = 1.f / delta;
	nSamples = FREQ_CACHE_SAMPLES;

	AllocateSamples(FREQ_CACHE_SAMPLES);

	// Fill samples with Frequency curve
	for(int i=0; i<FREQ_CACHE_SAMPLES; i++) {
		float w = (FREQ_CACHE_START + (delta*i));
		samples[i] = (sinf(w * freq + phase) + 1.0f) * 0.5f * refl;
	}

	Clamp();
}
