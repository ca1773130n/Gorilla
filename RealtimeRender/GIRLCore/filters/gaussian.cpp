/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   gaussian.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - gaussian.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// gaussian.cpp*
#include "gaussian.h"
#include "dynload.h"

using namespace girl;

// Gaussian Filter Method Definitions
float GaussianFilter::Evaluate(float x, float y) const {
	return Gaussian(x, expX) * Gaussian(y, expY);
}
Filter* GaussianFilter::CreateFilter(const ParamSet &ps) {
	// Find common filter parameters
	float xw = ps.FindOneFloat("xwidth", 2.);
	float yw = ps.FindOneFloat("ywidth", 2.);
	float alpha = ps.FindOneFloat("alpha", 2.f);
	return new GaussianFilter(xw, yw, alpha);
}

static DynamicLoader::RegisterFilter<GaussianFilter> r("gaussian");
