/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   BlinnPhong.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - BlinnPhong.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
// BlinnPhong material - based on previous/pbrt 'substrate' material using optional absorption

// glossy.cpp*
#include "bxdf.h"
#include "phongbxdf.h"
#include "anisotropic.h"
#include "paramset.h"
#include "dynload.h"
#include "GIRLPhong.h"

using namespace girl;

// BlinnPhong Method Definitions
BSDF *GIRLPhong::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const {
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	//if (bumpMap)
		//Bump(bumpMap, dgGeom, dgShading, &dgs);
	//else
		dgs = dgShading;
	PhongBxDF* phong = BSDF_ALLOC(tspack, PhongBxDF)(cosinePower->Evaluate(tspack, dgShading));
	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgShading, dgShading.nn);

	bsdf->Add(phong);

	/*
	SWCSpectrum d = Kd->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
	SWCSpectrum s = Ks->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
	float _n = n->Evaluate(tspack, dgs);

	bsdf->Add(BSDF_ALLOC(tspack, BlinnPhongBxDF)(d, s, BSDF_ALLOC(tspack, Blinn)(_n)));
	*/

	return bsdf;
}
Material* GIRLPhong::CreateMaterial(const Transform &xform, const TextureParams &mp) 
{
	boost::shared_ptr<Texture<SWCSpectrum> > specularColor = mp.GetSWCSpectrumTexture("specularColor", RGBColor(.5f));			
	boost::shared_ptr<Texture<float> > cosinePower = mp.GetFloatTexture("cosinePower", 1.0f);
	boost::shared_ptr<Texture<float> > reflectionSpecularity = mp.GetFloatTexture("reflectionSpecularity", .5f);
	boost::shared_ptr<Texture<float> > reflectionLimit = mp.GetFloatTexture("reflectionLimit", .0f);
	boost::shared_ptr<Texture<float> > reflectivity = mp.GetFloatTexture("reflectivity", .0f);
	boost::shared_ptr<Texture<SWCSpectrum> > reflectedColor = mp.GetSWCSpectrumTexture("reflectedColor", RGBColor(0.f));

	return new GIRLPhong(cosinePower, reflectionSpecularity, reflectionLimit, reflectivity, specularColor, reflectedColor, mp);
}

static DynamicLoader::RegisterMaterial<GIRLPhong> r("GirlPhong");
