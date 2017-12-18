/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   equalenergy.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - equalenergy.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// equalenergy.cpp*
#include "equalenergy.h"
#include "dynload.h"

using namespace girl;

// EqualEnergyTexture Method Definitions
Texture<float> * EqualEnergyTexture::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return new EqualEnergyFloatTexture<float>(tp.FindFloat("energy", 1.f));
}

Texture<SWCSpectrum> * EqualEnergyTexture::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return new EqualEnergySpectrumTexture<SWCSpectrum>(tp.FindFloat("energy", 1.f));
}

static DynamicLoader::RegisterFloatTexture<EqualEnergyTexture> r1("equalenergy");
static DynamicLoader::RegisterSWCSpectrumTexture<EqualEnergyTexture> r2("equalenergy");
