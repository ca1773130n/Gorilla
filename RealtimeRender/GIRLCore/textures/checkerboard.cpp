/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   checkerboard.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - checkerboard.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// checkerboard.cpp*
#include "checkerboard.h"
#include "dynload.h"


namespace girl
{
MethodType aaMethod;
}

using namespace girl;

// CheckerboardTexture Method Definitions
Texture<float> * Checkerboard::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	int dim = tp.FindInt("dimension", 2);
	if (dim != 2 && dim != 3) {
		//Error("%d dimensional checkerboard texture not supported", dim);
		std::stringstream ss;
		ss<<dim<<" dimensional checkerboard texture not supported";
		girlError(GIRL_UNIMPLEMENT,GIRL_ERROR,ss.str().c_str());
		return NULL;
	}
	boost::shared_ptr<Texture<float> > tex1 = tp.GetFloatTexture("tex1", 1.f);
	boost::shared_ptr<Texture<float> > tex2 = tp.GetFloatTexture("tex2", 0.f);
	if (dim == 2) {
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
		string aamode = tp.FindString("aamode");
		if (aamode == "") aamode = "closedform";
		return new Checkerboard2D<float>(map, tex1, tex2, aamode);
	}
	else {
		// Initialize 3D texture mapping _map_ from _tp_
		TextureMapping3D *map = new IdentityMapping3D(tex2world);
		// Apply texture specified transformation option for 3D mapping
		IdentityMapping3D *imap = (IdentityMapping3D*) map;
		imap->Apply3DTextureMappingOptions(tp);
		return new Checkerboard3D<float>(map, tex1, tex2);
	}
}

Texture<SWCSpectrum> * Checkerboard::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	int dim = tp.FindInt("dimension", 2);
	if (dim != 2 && dim != 3) {
		//Error("%d dimensional checkerboard texture not supported", dim);
		std::stringstream ss;
		ss<<dim<<" dimensional checkerboard texture not supported";
		girlError(GIRL_UNIMPLEMENT,GIRL_ERROR,ss.str().c_str());
		return NULL;
	}
	boost::shared_ptr<Texture<SWCSpectrum> > tex1 = tp.GetSWCSpectrumTexture("tex1", 1.f);
	boost::shared_ptr<Texture<SWCSpectrum> > tex2 = tp.GetSWCSpectrumTexture("tex2", 0.f);
	if (dim == 2) {
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
		string aamode = tp.FindString("aamode");
		if (aamode == "") aamode = "closedform";
		return new Checkerboard2D<SWCSpectrum>(map, tex1, tex2, aamode);
	}
	else {
		// Initialize 3D texture mapping _map_ from _tp_
		TextureMapping3D *map = new IdentityMapping3D(tex2world);
		// Apply texture specified transformation option for 3D mapping
		IdentityMapping3D *imap = (IdentityMapping3D*) map;
		imap->Apply3DTextureMappingOptions(tp);
		return new Checkerboard3D<SWCSpectrum>(map, tex1, tex2);
	}
}

static DynamicLoader::RegisterFloatTexture<Checkerboard> r1("checkerboard");
static DynamicLoader::RegisterSWCSpectrumTexture<Checkerboard> r2("checkerboard");
