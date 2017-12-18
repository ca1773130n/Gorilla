/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   regulardata.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - regulardata.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// regulardata.cpp*
#include "regulardata.h"
#include "dynload.h"

using namespace girl;

// RegularDataTexture Method Definitions
Texture<float> * RegularDataTexture::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	float start = tp.FindFloat("start", 380.f);
	float end = tp.FindFloat("end", 720.f);
	int dataCount;
	const float *data = tp.FindFloats("data", &dataCount);
	return new RegularDataFloatTexture<float>(1.f);
}

Texture<SWCSpectrum> * RegularDataTexture::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	float start = tp.FindFloat("start", 380.f);
	float end = tp.FindFloat("end", 720.f);
	int dataCount;
	const float *data = tp.FindFloats("data", &dataCount);
	return new RegularDataSpectrumTexture<SWCSpectrum>(start, end, dataCount, data);
}

static DynamicLoader::RegisterFloatTexture<RegularDataTexture> r1("regulardata");
static DynamicLoader::RegisterSWCSpectrumTexture<RegularDataTexture> r2("regulardata");
