/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   equalspd.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - equalspd.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// equalspd.cpp*
#include "equalspd.h"

using namespace girl;

void EqualSPD::init(float p) {
	power = p;

	lambdaMin = EQ_CACHE_START;
	lambdaMax = EQ_CACHE_END;
	delta = (EQ_CACHE_END - EQ_CACHE_START) / (EQ_CACHE_SAMPLES-1);
    invDelta = 1.f / delta;
	nSamples = EQ_CACHE_SAMPLES;

	AllocateSamples(EQ_CACHE_SAMPLES);

	// Fill sample with power value
	for(int i=0; i<EQ_CACHE_SAMPLES; i++) {
		samples[i] = power;
	}

	Clamp();
}
