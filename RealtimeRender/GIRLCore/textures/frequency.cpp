/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   frequency.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - frequency.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// frequency.cpp*
#include "frequency.h"
#include "dynload.h"

using namespace girl;

// FrequencyTexture Method Definitions
Texture<float> * FrequencyTexture::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return new FrequencyFloatTexture<float>(tp.FindFloat("energy", 1.f));
}

Texture<SWCSpectrum> * FrequencyTexture::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return new FrequencySpectrumTexture<SWCSpectrum>(tp.FindFloat("freq", .03f),
			tp.FindFloat("phase", .5f), tp.FindFloat("energy", 1.f));
}

static DynamicLoader::RegisterFloatTexture<FrequencyTexture> r1("frequency");
static DynamicLoader::RegisterSWCSpectrumTexture<FrequencyTexture> r2("frequency");
