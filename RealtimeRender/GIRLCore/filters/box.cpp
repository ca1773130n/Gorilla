/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   box.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - box.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// box.cpp*
#include "box.h"
#include "dynload.h"

using namespace girl;

// Box Filter Method Definitions
float BoxFilter::Evaluate(float x, float y) const {
	return 1.;
}
Filter* BoxFilter::CreateFilter(const ParamSet &ps) {
	float xw = ps.FindOneFloat("xwidth", .5f);
	float yw = ps.FindOneFloat("ywidth", .5f);
	return new BoxFilter(xw, yw);
}

static DynamicLoader::RegisterFilter<BoxFilter> r("box");
