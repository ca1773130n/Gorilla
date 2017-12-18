/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   mattetranslucent.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - mattetranslucent.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// mattetranslucent.cpp*
#include "mattetranslucent.h"
#include "bxdf.h"
#include "lambertian.h"
#include "orennayar.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// Matte Method Definitions
BSDF *MatteTranslucent::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom,
		const DifferentialGeometry &dgShading, float u) const {
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;

	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);
    // NOTE - lordcrc - changed clamping to 0..1 to avoid >1 reflection
	SWCSpectrum R = Kr->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
	SWCSpectrum T = Kt->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
	float sig = Clamp(sigma->Evaluate(tspack, dgs), 0.f, 90.f);

	if (!R.Black()) {
		if (sig == 0.)
			bsdf->Add(BSDF_ALLOC(tspack, Lambertian)(R));
		else
			bsdf->Add(BSDF_ALLOC(tspack, OrenNayar)(R, sig));
	}
	if (!T.Black()) {
		BxDF *base;
		if (sig == 0.)
			base = BSDF_ALLOC(tspack, Lambertian)(T);
		else
			base = BSDF_ALLOC(tspack, OrenNayar)(T, sig);
		bsdf->Add(BSDF_ALLOC(tspack, BRDFToBTDF)(base));
	}
	return bsdf;
}
Material* MatteTranslucent::CreateMaterial(const Transform &xform,
		const TextureParams &mp) {
	boost::shared_ptr<Texture<SWCSpectrum> > Kr = mp.GetSWCSpectrumTexture("Kr", RGBColor(1.f));
	boost::shared_ptr<Texture<SWCSpectrum> > Kt = mp.GetSWCSpectrumTexture("Kt", RGBColor(1.f));
	boost::shared_ptr<Texture<float> > sigma = mp.GetFloatTexture("sigma", 0.f);
	boost::shared_ptr<Texture<float> > bumpMap = mp.GetFloatTexture("bumpmap");
	return new MatteTranslucent(Kr, Kt, sigma, bumpMap);
}

static DynamicLoader::RegisterMaterial<MatteTranslucent> r("mattetranslucent");