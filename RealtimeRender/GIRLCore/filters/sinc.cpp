/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   sinc.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - sinc.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// sinc.cpp*
#include "sinc.h"
#include "dynload.h"

using namespace girl;

// Sinc Filter Method Definitions
float LanczosSincFilter::Evaluate(float x, float y) const{
	return Sinc1D(x * invXWidth) * Sinc1D(y * invYWidth);
}
float LanczosSincFilter::Sinc1D(float x) const {
	x = fabsf(x);
	if (x < 1e-5) return 1.f;
	if (x > 1.)   return 0.f;
	x *= M_PI;
	float sinc = sinf(x * tau) / (x * tau);
	float lanczos = sinf(x) / x;
	return sinc * lanczos;
}
Filter* LanczosSincFilter::CreateFilter(const ParamSet &ps) {
	float xw = ps.FindOneFloat("xwidth", 4.);
	float yw = ps.FindOneFloat("ywidth", 4.);
	float tau = ps.FindOneFloat("tau", 3.f);
	return new LanczosSincFilter(xw, yw, tau);
}

static DynamicLoader::RegisterFilter<LanczosSincFilter> r("sinc");
