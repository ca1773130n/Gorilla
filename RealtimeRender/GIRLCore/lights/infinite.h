/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   infinite.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - infinite.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// infinite.cpp*
#include "girl.h"
#include "light.h"
#include "texture.h"
#include "shape.h"
#include "scene.h"
#include "mipmap.h"

namespace girl
{

// InfiniteAreaLight Declarations
class GIRLCOREDLL  InfiniteAreaLight : public Light {
public:
	// InfiniteAreaLight Public Methods
	InfiniteAreaLight(const Transform &light2world, const RGBColor &l, int ns, const string &texmap,
		EnvironmentMapping *m, float gain, float gamma);
	~InfiniteAreaLight();
	SWCSpectrum Power(const TsPack *tspack, const Scene *scene) const {
		Point worldCenter;
		float worldRadius;
		scene->WorldBound().BoundingSphere(&worldCenter,
		                                    &worldRadius);
		RGBColor L = Lbase;
		if (radianceMap != NULL)
			L *= radianceMap->Lookup(.5f, .5f, .5f);

		return SWCSpectrum(tspack, SPDbase) * SWCSpectrum(tspack, L) * (M_PI * worldRadius * worldRadius);
	}
	bool IsDeltaLight() const { return false; }
	SWCSpectrum Le(const TsPack *tspack, const RayDifferential &r) const;
	SWCSpectrum Le(const TsPack *tspack, const Scene *scene, const Ray &r,
		const Normal &n, BSDF **bsdf, float *pdf, float *pdfDirect) const;
	SWCSpectrum Sample_L(const TsPack *tspack, const Point &p, const Normal &n,
		float u1, float u2, float u3, Vector *wi, float *pdf,
		VisibilityTester *visibility) const;
	SWCSpectrum Sample_L(const TsPack *tspack, const Point &p, float u1, float u2, float u3,
		Vector *wi, float *pdf, VisibilityTester *visibility) const;
	SWCSpectrum Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2,
			float u3, float u4, Ray *ray, float *pdf) const;
	float Pdf(const Point &, const Normal &, const Vector &) const;
	float Pdf(const Point &, const Vector &) const;
	bool Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2, float u3, BSDF **bsdf, float *pdf, SWCSpectrum *Le) const;
	bool Sample_L(const TsPack *tspack, const Scene *scene, const Point &p, const Normal &n, float u1, float u2, float u3, BSDF **bsdf, float *pdf, float *pdfDirect, VisibilityTester *visibility, SWCSpectrum *Le) const;
	
	static Light *CreateLight(const Transform &light2world,
		const ParamSet &paramSet);
private:
	// InfiniteAreaLight Private Data
	SPD *SPDbase;
	RGBColor Lbase;
	MIPMap<RGBColor> *radianceMap;
	EnvironmentMapping *mapping;
};

}//namespace girl


