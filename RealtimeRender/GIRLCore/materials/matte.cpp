/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   matte.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - matte.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// matte.cpp*
#include "matte.h"
#include "bxdf.h"
#include "lambertian.h"
#include "orennayar.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// Matte Method Definitions
BSDF *Matte::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom,
		const DifferentialGeometry &dgShading, float u) const {
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;
	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);
	// Evaluate textures for _Matte_ material and allocate BRDF
    // NOTE - lordcrc - changed clamping to 0..1 to avoid >1 reflection
	SWCSpectrum r = Kd->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
	float sig = Clamp(sigma->Evaluate(tspack, dgs), 0.f, 90.f);
	if (sig == 0.)
		bsdf->Add(BSDF_ALLOC(tspack, Lambertian)(r));
	else
		bsdf->Add(BSDF_ALLOC(tspack, OrenNayar)(r, sig));
	return bsdf;
}
Material* Matte::CreateMaterial(const Transform &xform,
		const TextureParams &mp) {
	boost::shared_ptr<Texture<SWCSpectrum> > Kd = mp.GetSWCSpectrumTexture("Kd", RGBColor(1.f));
	boost::shared_ptr<Texture<float> > sigma = mp.GetFloatTexture("sigma", 0.f);
	boost::shared_ptr<Texture<float> > bumpMap = mp.GetFloatTexture("bumpmap");
	return new Matte(Kd, sigma, bumpMap);
}

static DynamicLoader::RegisterMaterial<Matte> r("matte");
