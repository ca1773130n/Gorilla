/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   Ward.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - Ward.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#include "bxdf.h"
#include "lambertian.h"
#include "paramset.h"
#include "dynload.h"
#include "Ward.h"

using namespace girl;

// Ward Method Definitions
BSDF *Ward::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const {
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;

	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);

	SWCSpectrum kd = Kd->Evaluate(tspack, dgs).Clamp();
	SWCSpectrum ks = Ks->Evaluate(tspack, dgs).Clamp();
	float ax = Ax->Evaluate(tspack, dgs);
	float ay = Ay->Evaluate(tspack, dgs);
	float gamma = Gamma->Evaluate(tspack, dgs);

	if(!ks.Black()) {
		if (bWardDuer)
			bsdf->Add(BSDF_ALLOC(tspack, WardBRDF)(ks, ax, ay, gamma));
		else
			bsdf->Add(BSDF_ALLOC(tspack, WardBRDF)(ks, ax, ay, gamma));
	}

	bsdf->Add(BSDF_ALLOC(tspack, Lambertian)(kd));

	return bsdf;
}
// Ward Dynamic Creation Routine
Material * Ward::CreateMaterial(const Transform &xform,
                                    const TextureParams &mp) {
	boost::shared_ptr<Texture<SWCSpectrum> > Kd = mp.GetSWCSpectrumTexture("Kd", RGBColor(0.f));
	boost::shared_ptr<Texture<SWCSpectrum> > Ks = mp.GetSWCSpectrumTexture("Ks", RGBColor(0.f));
	boost::shared_ptr<Texture<float> > bumpMap = mp.GetFloatTexture("bumpmap", 0.f);
	boost::shared_ptr<Texture<float> > Ax = mp.GetFloatTexture("Ax", .1f);
	boost::shared_ptr<Texture<float> > Ay = mp.GetFloatTexture("Ay", .1f);
	boost::shared_ptr<Texture<float> > gamma = mp.GetFloatTexture("Gamma", 0.0f);
	bool wardDuer = mp.FindBool("wardDuer", false);

	return new Ward(Kd, Ks, bumpMap, Ax, Ay, gamma, wardDuer);
}

static DynamicLoader::RegisterMaterial<Ward> r("ward");