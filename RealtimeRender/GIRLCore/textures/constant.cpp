/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   constant.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - constant.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// constant.cpp*
#include "constant.h"
#include "dynload.h"

using namespace girl;

// ConstantTexture Method Definitions
Texture<float> * Constant::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
			
			Texture<float> *pTex = new ConstantFloatTexture<float>(tp.FindFloat("value", 1.f));
			pTex->textureType = TEXTURE_CONSTANT;
			pTex->filepath = "";
	return pTex;
}

Texture<SWCSpectrum> * Constant::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
			Texture<SWCSpectrum> *pTex = new ConstantRGBColorTexture<SWCSpectrum>(tp.FindRGBColor("value", RGBColor(1.f)));
			pTex->textureType = TEXTURE_CONSTANT;
			pTex->filepath = "";
	return pTex;
}

static DynamicLoader::RegisterFloatTexture<Constant> r1("constant");
static DynamicLoader::RegisterSWCSpectrumTexture<Constant> r2("constant");
