/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   He.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - He.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#include "bxdf.h"
#include "paramset.h"
#include "dynload.h"
#include "He.h"

using namespace girl;

// He Method Definitions
BSDF *He::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const {
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;

	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);

	SWCSpectrum kd = Kd->Evaluate(tspack, dgs).Clamp();
	SWCSpectrum ks = Ks->Evaluate(tspack, dgs).Clamp();
	float tauValue = tau->Evaluate(tspack, dgs);
	float sigmaValue = sigma->Evaluate(tspack, dgs);
	float nrealValue = nreal->Evaluate(tspack, dgs);
	float nimagValue = nimag->Evaluate(tspack, dgs);

	bsdf->Add(BSDF_ALLOC(tspack, HTSGBRDF)(kd, ks, tauValue, sigmaValue, nrealValue, nimagValue));

	return bsdf;
}
// He Dynamic Creation Routine
Material * He::CreateMaterial(const Transform &xform,
                                    const TextureParams &mp) {
	boost::shared_ptr<Texture<SWCSpectrum> > Kd = mp.GetSWCSpectrumTexture("Kd", RGBColor(0.6f));
	boost::shared_ptr<Texture<SWCSpectrum> > Ks = mp.GetSWCSpectrumTexture("Ks", RGBColor(0.4f));
	boost::shared_ptr<Texture<float> > bumpMap = mp.GetFloatTexture("bumpmap", 0.f);
	boost::shared_ptr<Texture<float> > tauValue = mp.GetFloatTexture("tau", 20.f);
	boost::shared_ptr<Texture<float> > sigmaValue = mp.GetFloatTexture("sigma", 1.f);
	boost::shared_ptr<Texture<float> > nrealValue = mp.GetFloatTexture("nreal", 0.2f);
	boost::shared_ptr<Texture<float> > nimagValue = mp.GetFloatTexture("nimag", 0.0f);

	return new He(Kd, Ks, bumpMap, tauValue, sigmaValue, nrealValue, nimagValue);
}

static DynamicLoader::RegisterMaterial<He> r("he");