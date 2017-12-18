/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   blackbody.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - blackbody.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// blackbody.cpp*
#include "blackbody.h"
#include "dynload.h"

using namespace girl;

// BlackBodyTexture Method Definitions
Texture<float> * BlackBodyTexture::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return new BlackBodyFloatTexture<float>(
		tp.FindFloat("temperature", 6500.f));
}

Texture<SWCSpectrum> * BlackBodyTexture::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return new BlackBodySpectrumTexture<SWCSpectrum>(
		tp.FindFloat("temperature", 6500.f));
}

static DynamicLoader::RegisterFloatTexture<BlackBodyTexture> r1("blackbody");
static DynamicLoader::RegisterSWCSpectrumTexture<BlackBodyTexture> r2("blackbody");
