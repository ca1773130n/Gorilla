/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   imagemap.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - imagemap.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// imagemap.cpp*
#include "imagemap.h"
#include "dynload.h"

using namespace girl;

Texture<float> *ImageFloatTexture::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	// Initialize 2D texture mapping _map_ from _tp_
	TextureMapping2D *map = NULL;
	
	string sFilterType = tp.FindString("filtertype");
	ImageTextureFilterType filterType = BILINEAR;
	if ((sFilterType == "") || (sFilterType == "bilinear")) {
		filterType = BILINEAR;
	} else if (sFilterType == "mipmap_trilinear") {
		filterType = MIPMAP_TRILINEAR;
	} else if (sFilterType == "mipmap_ewa") {
		filterType = MIPMAP_EWA;
	} else if (sFilterType == "nearest") {
		filterType = NEAREST;
	}

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
		ss<<"2D texture mapping  '"<<type<<"' unknown";
		girlError(GIRL_BADTOKEN,GIRL_ERROR,ss.str().c_str());
		map = new UVMapping2D;
	}

	// Initialize _ImageTexture_ parameters
	float maxAniso = tp.FindFloat("maxanisotropy", 8.f);
	string wrap = tp.FindString("wrap");
	ImageWrap wrapMode = TEXTURE_REPEAT;
	if (wrap == "" || wrap == "repeat") wrapMode = TEXTURE_REPEAT;
	else if (wrap == "black") wrapMode = TEXTURE_BLACK;
	else if (wrap == "clamp") wrapMode = TEXTURE_CLAMP;

	float gain = tp.FindFloat("gain", 1.0f);
	float gamma = tp.FindFloat("gamma", 1.0f);

	string filename = tp.FindString("filename");
	int discardmm = tp.FindInt("discardmipmaps", 0);

	ImageFloatTexture *tex = new ImageFloatTexture(map, filterType,
			filename, maxAniso, wrapMode, gain, gamma);

	if ((discardmm > 0) &&
			((filterType == MIPMAP_TRILINEAR) || (filterType == MIPMAP_EWA))) {
		tex->discardMipmaps(discardmm);

		std::stringstream ss;
		ss<<"Discarded " << discardmm << " mipmap levels";
		girlError(GIRL_NOERROR,GIRL_INFO,ss.str().c_str());
	}

	std::stringstream ss;
	ss<<"Memory used for imagemap '" << filename << "': " <<
		(tex->getMemoryUsed() / 1024) << "KBytes\n";
	//girlError(GIRL_NOERROR,GIRL_INFO,ss.str().c_str());
	girlProgressMsgPrint(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

	return tex;
}

Texture<SWCSpectrum> *ImageSpectrumTexture::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	// Initialize 2D texture mapping _map_ from _tp_
	TextureMapping2D *map = NULL;
	
	string sFilterType = tp.FindString("filtertype");
	ImageTextureFilterType filterType = BILINEAR;
	if ((sFilterType == "") || (sFilterType == "bilinear")) {
		filterType = BILINEAR;
	} else if (sFilterType == "mipmap_trilinear") {
		filterType = MIPMAP_TRILINEAR;
	} else if (sFilterType == "mipmap_ewa") {
		filterType = MIPMAP_EWA;
	} else if (sFilterType == "nearest") {
		filterType = NEAREST;
	}

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
		ss<<"2D texture mapping  '"<<type<<"' unknown";
		girlError(GIRL_BADTOKEN,GIRL_ERROR,ss.str().c_str());
		map = new UVMapping2D;
	}

	// Initialize _ImageTexture_ parameters
	float maxAniso = tp.FindFloat("maxanisotropy", 8.f);
	string wrap = tp.FindString("wrap");
	ImageWrap wrapMode = TEXTURE_REPEAT;
	if (wrap == "" || wrap == "repeat") wrapMode = TEXTURE_REPEAT;
	else if (wrap == "black") wrapMode = TEXTURE_BLACK;
	else if (wrap == "clamp") wrapMode = TEXTURE_CLAMP;

	float gain = tp.FindFloat("gain", 1.0f);
	float gamma = tp.FindFloat("gamma", 1.0f);

	string filename = tp.FindString("filename");
	int discardmm = tp.FindInt("discardmipmaps", 0);

	ImageSpectrumTexture *tex = new ImageSpectrumTexture(map, filterType,
			filename, maxAniso, wrapMode, gain, gamma);

	if ((discardmm > 0) &&
			((filterType == MIPMAP_TRILINEAR) || (filterType == MIPMAP_EWA))) {
		tex->discardMipmaps(discardmm);

		std::stringstream ss;
		ss<<"Discarded " << discardmm << " mipmap levels";
		girlError(GIRL_NOERROR,GIRL_INFO,ss.str().c_str());
	}

	std::stringstream ss;
	ss<<"Memory used for imagemap '" << filename << "': " <<
		(tex->getMemoryUsed() / 1024) << "KBytes\n";
	//girlError(GIRL_NOERROR,GIRL_INFO,ss.str().c_str());
	girlProgressMsgPrint(GIRL_NOERROR,GIRL_INFO,ss.str().c_str());

	return tex;
}

static DynamicLoader::RegisterFloatTexture<ImageFloatTexture> r1("imagemap");
static DynamicLoader::RegisterSWCSpectrumTexture<ImageSpectrumTexture> r2("imagemap");
