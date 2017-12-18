/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   mirror.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - mirror.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// mirror.cpp*
#include "mirror.h"
#include "bxdf.h"
#include "specularreflection.h"
#include "fresnelnoop.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// Mirror Method Definitions
BSDF *Mirror::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const {
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;

	float flm = film->Evaluate(tspack, dgs);
	float flmindex = filmindex->Evaluate(tspack, dgs);

	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);
    // NOTE - lordcrc - changed clamping to 0..1 to avoid >1 reflection
	SWCSpectrum R = Kr->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
	if (!R.Black())
		bsdf->Add(BSDF_ALLOC(tspack, SpecularReflection)(R,
			BSDF_ALLOC(tspack, FresnelNoOp)(), flm, flmindex));
	return bsdf;
}
Material* Mirror::CreateMaterial(const Transform &xform,
		const TextureParams &mp) {
	boost::shared_ptr<Texture<SWCSpectrum> > Kr = mp.GetSWCSpectrumTexture("Kr", RGBColor(1.f));
	boost::shared_ptr<Texture<float> > film = mp.GetFloatTexture("film", 0.f);				// Thin film thickness in nanometers
	boost::shared_ptr<Texture<float> > filmindex = mp.GetFloatTexture("filmindex", 1.5f);				// Thin film index of refraction
	boost::shared_ptr<Texture<float> > bumpMap = mp.GetFloatTexture("bumpmap");
	return new Mirror(Kr, film, filmindex, bumpMap);
}

static DynamicLoader::RegisterMaterial<Mirror> r("mirror");
