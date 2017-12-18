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
#include "GIRLBlinn.h"
#include "BlinnBRDF.h"

using namespace girl;

// BlinnPhong Method Definitions
BSDF *GIRLBlinn::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const
{
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	//if (bumpMap)
	//	Bump(bumpMap, dgGeom, dgShading, &dgs);
	//else
		dgs = dgShading;

	Fresnel** fresnel = (Fresnel **)BSDF::Alloc(tspack, 1 * sizeof(Fresnel *));

	BlinnBRDF* blinn = BSDF_ALLOC(tspack, BlinnBRDF)(specularColor->Evaluate(tspack, dgs), eccentricity->Evaluate(tspack, dgs), specularRollOff->Evaluate(tspack, dgs), *fresnel);
	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgShading, dgShading.nn);
	bsdf->Add(blinn);

	/*
	SWCSpectrum d = Kd->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
	SWCSpectrum s = Ks->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
	float _n = n->Evaluate(tspack, dgs);

	bsdf->Add(BSDF_ALLOC(tspack, BlinnPhongBxDF)(d, s, BSDF_ALLOC(tspack, Blinn)(_n)));
	*/

	return bsdf;
}
Material* GIRLBlinn::CreateMaterial(const Transform &xform, const TextureParams &mp) 
{
	boost::shared_ptr<Texture<SWCSpectrum> > specularColor = mp.GetSWCSpectrumTexture("specularColor", RGBColor(.5f));	
	boost::shared_ptr<Texture<float> > specularRollOff = mp.GetFloatTexture("specularRollOff", 1.f);	
	boost::shared_ptr<Texture<float> > eccentricity = mp.GetFloatTexture("eccentricity", 1.0f);
	return new GIRLBlinn( specularColor, specularRollOff, eccentricity, mp );
}

static DynamicLoader::RegisterMaterial<GIRLBlinn> r("GirlBlinn");
