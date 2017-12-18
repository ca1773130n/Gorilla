/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   spd.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - spd.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// spd.cpp*
#include "girl.h"
#include "spd.h"
#include "memory.h"

using namespace girl;

void SPD::AllocateSamples(int n) {
	 // Allocate memory for samples
	samples = (float *)
		AllocAligned(n * sizeof(float));
}

void SPD::FreeSamples() {
	 // Free Allocate memory for samples
	FreeAligned(samples);
}

void SPD::Normalize() {
	float max = 0.f;

	for(int i=0; i<nSamples; i++)
		if(samples[i] > max)
			max = samples[i];

	float scale = 1.f/max;

	for(int i=0; i<nSamples; i++)
		samples[i] *= scale;
}

void SPD::Clamp() {
	for(int i=0; i<nSamples; i++) {
		if(samples[i] < 0.f) samples[i] = 0.f;
		if(samples[i] > INFINITY) samples[i] = INFINITY;
	}
}

void SPD::Scale(float s) {
	for(int i=0; i<nSamples; i++)
		samples[i] *= s;
}

void SPD::Whitepoint(float temp) {
	vector<float> bbvals;

	// Fill bbvals with BB curve
	for(int i=0; i<nSamples; i++) {
		float w = 1e-9f * (lambdaMin + (delta*i));
		// Compute blackbody power for wavelength w and temperature temp
		bbvals.push_back(4e-9f * (3.74183e-16f * powf(w, -5.f))
				/ (expf(1.4388e-2f / (w * temp)) - 1.f));
	}

	// Normalize
	float max = 0.f;
	for(int i=0; i<nSamples; i++)
		if(bbvals[i] > max)
			max = bbvals[i];
	float scale = 1.f/max;
	for(int i=0; i<nSamples; i++)
		bbvals[i] *= scale;

	// Multiply
	for(int i=0; i<nSamples; i++)
		samples[i] *= bbvals[i];

	bbvals.clear();
}

#include "data/xyzbasis.h"

float SPD::y() {
	float y = 0.f;

	for(int i=0; i<nSamples; i++) {
		float waveln = (lambdaMin + (delta*i));
		// Interpolate Y Conversion weights
		const float w0 = waveln - CIEstart;
		int i0 = Floor2Int(w0);
		const float b0 = w0 - i0;
		y += samples[i] * Lerp(b0, CIE_Y[i0], CIE_Y[i0 + 1]);
	}

	return y/nSamples;
}
