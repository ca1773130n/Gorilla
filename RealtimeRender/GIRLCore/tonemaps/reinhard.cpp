/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   reinhard.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - reinhard.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

/*
 * Reinhard Tonemapping (Indigo compatible) class
 *
 * Uses Indigo compatible parameters.
 *
 * 30/09/07 - Radiance - Initial Version
 */

#include "reinhard.h"
#include "dynload.h"

using namespace girl;

// ReinhardOp Method Definitions
ReinhardOp::ReinhardOp(float prS, float poS, float b)
{
	pre_scale = prS;
	post_scale = poS;
	burn = b;
}

void ReinhardOp::Map(vector<Color> &xyz, int xRes, int yRes, float maxDisplayY) const
{
	const float alpha = .1f;
	const int numPixels = xRes * yRes;

	// Compute world adaptation luminance, _Ywa_
	float Ywa = 0.f;
	for (int i = 0; i < numPixels; ++i)
		if (xyz[i].c[1] > 0.f) Ywa += xyz[i].c[1];
	Ywa /= numPixels;
	if (Ywa == 0.f)
		Ywa = 1.f;

	const float Yw = pre_scale * alpha * burn;
	const float invY2 = 1.f / (Yw * Yw);
	const float pScale = post_scale * pre_scale * alpha / Ywa;

	for (int i = 0; i < numPixels; ++i) {
		float ys = xyz[i].c[1];
		xyz[i] *= pScale * (1.f + ys * invY2) / (1.f + ys);
	}
}

ToneMap * ReinhardOp::CreateToneMap(const ParamSet &ps) {
	float pre_scale = ps.FindOneFloat("prescale", 1.f);
	float post_scale = ps.FindOneFloat("postscale", 1.f);
	float burn = ps.FindOneFloat("burn", 7.f);
	return new ReinhardOp(pre_scale, post_scale, burn);
}

static DynamicLoader::RegisterToneMap<ReinhardOp> r("reinhard");
