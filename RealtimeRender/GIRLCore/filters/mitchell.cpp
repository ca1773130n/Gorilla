/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   mitchell.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - mitchell.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// mitchell.cpp*
#include "mitchell.h"
#include "dynload.h"

using namespace girl;

// Mitchell Filter Method Definitions
float MitchellFilter::Evaluate(float x, float y) const {
	return Mitchell1D(x * invXWidth) *
		Mitchell1D(y * invYWidth);
}
Filter* MitchellFilter::CreateFilter(const ParamSet &ps) {
	// Find common filter parameters
	float xw = ps.FindOneFloat("xwidth", 2.);
	float yw = ps.FindOneFloat("ywidth", 2.);
	float B = ps.FindOneFloat("B", 1.f/3.f);
	float C = ps.FindOneFloat("C", 1.f/3.f);
	return new MitchellFilter(B, C, xw, yw);
}

static DynamicLoader::RegisterFilter<MitchellFilter> r("mitchell");
