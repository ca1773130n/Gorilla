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
#include "blinn.h"
#include "anisotropic.h"
#include "paramset.h"
#include "dynload.h"
#include "BlinnPhong.h"

using namespace girl;

// BlinnPhong Method Definitions
BSDF *BlinnPhong::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const {
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;
	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);

	SWCSpectrum d = Kd->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
	SWCSpectrum s = Ks->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
	float _n = n->Evaluate(tspack, dgs);

	bsdf->Add(BSDF_ALLOC(tspack, BlinnPhongBxDF)(d, s, BSDF_ALLOC(tspack, Blinn)(_n)));

	return bsdf;
}
Material* BlinnPhong::CreateMaterial(const Transform &xform,
		const TextureParams &mp) {
	boost::shared_ptr<Texture<SWCSpectrum> > Kd = mp.GetSWCSpectrumTexture("Kd", RGBColor(.5f));
	boost::shared_ptr<Texture<SWCSpectrum> > Ks = mp.GetSWCSpectrumTexture("Ks", RGBColor(.5f));
	boost::shared_ptr<Texture<float> > n = mp.GetFloatTexture("n", .0f);
	boost::shared_ptr<Texture<float> > bumpMap = mp.GetFloatTexture("bumpmap");
	return new BlinnPhong(Kd, Ks, n, bumpMap);
}

static DynamicLoader::RegisterMaterial<BlinnPhong> r("blinnphong");
