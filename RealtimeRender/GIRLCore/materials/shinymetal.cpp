/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   shinymetal.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - shinymetal.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// shinymetal.cpp*
#include "shinymetal.h"
#include "bxdf.h"
#include "fresnelconductor.h"
#include "blinn.h"
#include "anisotropic.h"
#include "microfacet.h"
#include "specularreflection.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// ShinyMetal Method Definitions
BSDF *ShinyMetal::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float) const {
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;
	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);
	SWCSpectrum spec = Ks->Evaluate(tspack, dgs).Clamp();
	SWCSpectrum R = Kr->Evaluate(tspack, dgs).Clamp();

	float u = nu->Evaluate(tspack, dgs);
	float v = nv->Evaluate(tspack, dgs);

	float flm = film->Evaluate(tspack, dgs);
	float flmindex = filmindex->Evaluate(tspack, dgs);

	MicrofacetDistribution *md;
	if(u == v)
		md = BSDF_ALLOC(tspack, Blinn)(1.f / u);
	else
		md = BSDF_ALLOC(tspack, Anisotropic)(1.f/u, 1.f/v);

	SWCSpectrum k = 0.;
	Fresnel *frMf = BSDF_ALLOC(tspack, FresnelConductor)(FresnelApproxEta(spec), k);
	Fresnel *frSr = BSDF_ALLOC(tspack, FresnelConductor)(FresnelApproxEta(R), k);
	bsdf->Add(BSDF_ALLOC(tspack, Microfacet)(1., frMf, md));
	bsdf->Add(BSDF_ALLOC(tspack, SpecularReflection)(1., frSr, flm, flmindex));
	return bsdf;
}
Material* ShinyMetal::CreateMaterial(const Transform &xform,
		const TextureParams &mp) {
	boost::shared_ptr<Texture<SWCSpectrum> > Kr = mp.GetSWCSpectrumTexture("Kr", RGBColor(1.f));
	boost::shared_ptr<Texture<SWCSpectrum> > Ks = mp.GetSWCSpectrumTexture("Ks", RGBColor(1.f));
	boost::shared_ptr<Texture<float> > uroughness = mp.GetFloatTexture("uroughness", .1f);
	boost::shared_ptr<Texture<float> > vroughness = mp.GetFloatTexture("vroughness", .1f);
	boost::shared_ptr<Texture<float> > film = mp.GetFloatTexture("film", 0.f);				// Thin film thickness in nanometers
	boost::shared_ptr<Texture<float> > filmindex = mp.GetFloatTexture("filmindex", 1.5f);	// Thin film index of refraction
	boost::shared_ptr<Texture<float> > bumpMap = mp.GetFloatTexture("bumpmap");
	return new ShinyMetal(Ks, uroughness, vroughness, film, filmindex, Kr, bumpMap);
}

static DynamicLoader::RegisterMaterial<ShinyMetal> r("shinymetal");
