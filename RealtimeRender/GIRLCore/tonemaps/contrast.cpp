/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   contrast.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - contrast.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// contrast.cpp*
#include "contrast.h"
#include "dynload.h"

using namespace girl;

// ContrastOp Method Definitions
void ContrastOp::Map(vector<Color> &xyz, int xRes, int yRes,
		float maxDisplayY) const {
	// Compute world adaptation luminance, _Ywa_
	float Ywa = 0.;
	for (int i = 0; i < xRes * yRes; ++i)
		if (xyz[i].y() > 0) Ywa += logf(xyz[i].y());
	Ywa = expf(683.f * Ywa / (xRes * yRes));
	// Compute contrast-preserving scalefactor, _s_
	float s = powf((1.219f + powf(displayAdaptationY, 0.4f)) /
		(1.219f + powf(Ywa, 0.4f)), 2.5f) * 683.f / maxDisplayY;
	for (int i = 0; i < xRes*yRes; ++i)
		xyz[i] *= s;
}
ToneMap * ContrastOp::CreateToneMap(const ParamSet &ps) {
	float day = ps.FindOneFloat("displayadaptationY", 50.f);
	return new ContrastOp(day);
}

static DynamicLoader::RegisterToneMap<ContrastOp> r("contrast");
