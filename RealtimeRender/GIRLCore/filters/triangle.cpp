/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   triangle.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - triangle.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// triangle.cpp*
#include "triangle.h"
#include "dynload.h"

using namespace girl;

// Triangle Filter Method Definitions
float TriangleFilter::Evaluate(float x, float y) const {
	return max(0.f, xWidth - fabsf(x)) *
		max(0.f, yWidth - fabsf(y));
}
Filter* TriangleFilter::CreateFilter(const ParamSet &ps) {
	// Find common filter parameters
	float xw = ps.FindOneFloat("xwidth", 2.);
	float yw = ps.FindOneFloat("ywidth", 2.);
	return new TriangleFilter(xw, yw);
}

static DynamicLoader::RegisterFilter<TriangleFilter> r("triangle");
