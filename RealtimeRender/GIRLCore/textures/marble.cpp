/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   marble.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - marble.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// marble.cpp*
#include "marble.h"
#include "dynload.h"

using namespace girl;

// MarbleTexture Method Definitions
Texture<float> * MarbleTexture::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return NULL;
}

Texture<SWCSpectrum> * MarbleTexture::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	// Initialize 3D texture mapping _map_ from _tp_
	TextureMapping3D *map = new IdentityMapping3D(tex2world);
	// Apply texture specified transformation option for 3D mapping
	IdentityMapping3D *imap = (IdentityMapping3D*) map;
	imap->Apply3DTextureMappingOptions(tp);
	return new MarbleTexture(tp.FindInt("octaves", 8),
		tp.FindFloat("roughness", .5f),
		tp.FindFloat("scale", 1.f),
		tp.FindFloat("variation", .2f),
		map);
}

static DynamicLoader::RegisterFloatTexture<MarbleTexture> r1("marble");
static DynamicLoader::RegisterSWCSpectrumTexture<MarbleTexture> r2("marble");
