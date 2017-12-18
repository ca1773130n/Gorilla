/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   glossy.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - glossy.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// Glossy material - based on previous/pbrt 'substrate' material using optional absorption

// glossy.cpp*
#include "glossy.h"
#include "bxdf.h"
#include "fresnelblend.h"
#include "blinn.h"
#include "anisotropic.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// Glossy Method Definitions
BSDF *Glossy::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float) const {
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;
	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);
    // NOTE - lordcrc - changed clamping to 0..1 to avoid >1 reflection
	SWCSpectrum d = Kd->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
	SWCSpectrum s = Ks->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
	SWCSpectrum a = Ka->Evaluate(tspack, dgs).Clamp(0.f, 1.f);

	float u = nu->Evaluate(tspack, dgs);
	float v = nv->Evaluate(tspack, dgs);

	float ld = depth->Evaluate(tspack, dgs);

	if(u == v)
		bsdf->Add(BSDF_ALLOC(tspack, FresnelBlend)(d, s, a, ld, BSDF_ALLOC(tspack, Blinn)(1.f/u)));
	else
		bsdf->Add(BSDF_ALLOC(tspack, FresnelBlend)(d, s, a, ld, BSDF_ALLOC(tspack, Anisotropic)(1.f/u, 1.f/v)));

	return bsdf;
}
Material* Glossy::CreateMaterial(const Transform &xform,
		const TextureParams &mp) {
	boost::shared_ptr<Texture<SWCSpectrum> > Kd = mp.GetSWCSpectrumTexture("Kd", RGBColor(.5f));
	boost::shared_ptr<Texture<SWCSpectrum> > Ks = mp.GetSWCSpectrumTexture("Ks", RGBColor(.5f));
	boost::shared_ptr<Texture<SWCSpectrum> > Ka = mp.GetSWCSpectrumTexture("Ka", RGBColor(.0f));
	boost::shared_ptr<Texture<float> > d = mp.GetFloatTexture("d", .0f);
	boost::shared_ptr<Texture<float> > uroughness = mp.GetFloatTexture("uroughness", .1f);
	boost::shared_ptr<Texture<float> > vroughness = mp.GetFloatTexture("vroughness", .1f);
	boost::shared_ptr<Texture<float> > bumpMap = mp.GetFloatTexture("bumpmap");
	return new Glossy(Kd, Ks, Ka, d, uroughness, vroughness, bumpMap);
}

static DynamicLoader::RegisterMaterial<Glossy> r("glossy");
static DynamicLoader::RegisterMaterial<Glossy> r1("substrate"); // Backwards compatibility for 'substrate'
static DynamicLoader::RegisterMaterial<Glossy> r2("plastic");   // Backwards compaticility for removed 'plastic'
