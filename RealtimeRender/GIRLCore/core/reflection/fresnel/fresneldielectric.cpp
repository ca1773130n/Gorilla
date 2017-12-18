/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   fresneldielectric.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - fresneldielectric.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// fresneldielectric.cpp*
#include "fresneldielectric.h"
#include "spectrum.h"
#include "spectrumwavelengths.h"

using namespace girl;

void FresnelDielectric::Evaluate(const TsPack *tspack, float cosi, SWCSpectrum *const f) const {
	// Compute Fresnel reflectance for dielectric
	cosi = Clamp(cosi, -1.f, 1.f);
	// Compute indices of refraction for dielectric
	bool entering = cosi > 0.;
	float ei = eta_i, et = eta_t;

	if(cb != 0.) {
		// Handle dispersion using cauchy formula
		float w = tspack->swl->SampleSingle();
		et = eta_t + (cb * 1000000) / (w*w);
	}

	if (!entering)
		swap(ei, et);
	// Compute _sint_ using Snell's law
	float sint = ei/et * sqrtf(max(0.f, 1.f - cosi*cosi));
	if (sint > 1.) {
		// Handle total internal reflection
		*f = SWCSpectrum(1.);
	}
	else {
		float cost = sqrtf(max(0.f, 1.f - sint*sint));
		FrDiel(fabsf(cosi), cost, ei, et, f);
	}
}

