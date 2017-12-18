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

// GaussianTexture Method Definitions
Texture<float> * GaussianTexture::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return new GaussianFloatTexture<float>(tp.FindFloat("energy", 1.f));
}

Texture<SWCSpectrum> * GaussianTexture::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return new GaussianSpectrumTexture<SWCSpectrum>(tp.FindFloat("wavelength", 550.f),
			tp.FindFloat("width", 50.f), tp.FindFloat("energy", 1.f));
}

static DynamicLoader::RegisterFloatTexture<GaussianTexture> r1("gaussian");
static DynamicLoader::RegisterSWCSpectrumTexture<GaussianTexture> r2("gaussian");
