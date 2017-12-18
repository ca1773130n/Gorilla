/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   uv.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - uv.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// uv.cpp*
#include "uv.h"
#include "error.h"
#include "dynload.h"

using namespace girl;

// UVTexture Method Definitions
Texture<float> * UVTexture::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return NULL;
}

Texture<SWCSpectrum> * UVTexture::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	// Initialize 2D texture mapping _map_ from _tp_
	TextureMapping2D *map = NULL;
	string type = tp.FindString("mapping");
	if (type == "" || type == "uv") {
		float su = tp.FindFloat("uscale", 1.);
		float sv = tp.FindFloat("vscale", 1.);
		float du = tp.FindFloat("udelta", 0.);
		float dv = tp.FindFloat("vdelta", 0.);
		map = new UVMapping2D(su, sv, du, dv);
	}
	else if (type == "spherical") map = new SphericalMapping2D(tex2world.GetInverse());
	else if (type == "cylindrical") map = new CylindricalMapping2D(tex2world.GetInverse());
	else if (type == "planar")
		map = new PlanarMapping2D(tp.FindVector("v1", Vector(1,0,0)),
			tp.FindVector("v2", Vector(0,1,0)),
			tp.FindFloat("udelta", 0.f), tp.FindFloat("vdelta", 0.f));
	else {
		//Error("2D texture mapping \"%s\" unknown", type.c_str());
		std::stringstream ss;
		ss<<"2D texture mapping '"<<type<<"' unknown";
		girlError(GIRL_BADTOKEN,GIRL_ERROR,ss.str().c_str());
		map = new UVMapping2D;
	}
	return new UVTexture(map);
}

static DynamicLoader::RegisterFloatTexture<UVTexture> r1("uv");
static DynamicLoader::RegisterSWCSpectrumTexture<UVTexture> r2("uv");