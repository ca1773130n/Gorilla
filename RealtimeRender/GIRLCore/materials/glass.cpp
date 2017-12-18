/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   glass.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - glass.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// glass.cpp*
#include "glass.h"
#include "bxdf.h"
#include "specularreflection.h"
#include "speculartransmission.h"
#include "fresneldielectric.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// Glass Method Definitions
BSDF *Glass::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const {
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;
	// NOTE - lordcrc - Bugfix, pbrt tracker id 0000078: index of refraction swapped and not recorded
	float ior = index->Evaluate(tspack, dgs);
	float cb = cauchyb->Evaluate(tspack, dgs);

	float flm = film->Evaluate(tspack, dgs);
	float flmindex = filmindex->Evaluate(tspack, dgs);

	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn, ior);
    // NOTE - lordcrc - changed clamping to 0..1 to avoid >1 reflection
	SWCSpectrum R = Kr->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
	SWCSpectrum T = Kt->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
	if (!R.Black()) {
		Fresnel *fresnel = BSDF_ALLOC(tspack, FresnelDielectric)(1.f, ior);
		if (architectural)
			bsdf->Add(BSDF_ALLOC(tspack, ArchitecturalReflection)(R,
				fresnel, flm, flmindex));
		else
			bsdf->Add(BSDF_ALLOC(tspack, SpecularReflection)(R,
				fresnel, flm, flmindex));
	}
	if (!T.Black())
		bsdf->Add(BSDF_ALLOC(tspack, SpecularTransmission)(T, 1., ior, cb, architectural));
	return bsdf;
}
Material* Glass::CreateMaterial(const Transform &xform,
		const TextureParams &mp) {
	boost::shared_ptr<Texture<SWCSpectrum> > Kr = mp.GetSWCSpectrumTexture("Kr", RGBColor(1.f));
	boost::shared_ptr<Texture<SWCSpectrum> > Kt = mp.GetSWCSpectrumTexture("Kt", RGBColor(1.f));
	boost::shared_ptr<Texture<float> > index = mp.GetFloatTexture("index", 1.5f);
	boost::shared_ptr<Texture<float> > cbf = mp.GetFloatTexture("cauchyb", 0.f);				// Cauchy B coefficient
	boost::shared_ptr<Texture<float> > film = mp.GetFloatTexture("film", 0.f);				// Thin film thickness in nanometers
	boost::shared_ptr<Texture<float> > filmindex = mp.GetFloatTexture("filmindex", 1.5f);				// Thin film index of refraction
	bool archi = mp.FindBool("architectural", false);
	boost::shared_ptr<Texture<float> > bumpMap = mp.GetFloatTexture("bumpmap");
	return new Glass(Kr, Kt, index, cbf, film, filmindex, archi, bumpMap);
}

static DynamicLoader::RegisterMaterial<Glass> r("glass");
