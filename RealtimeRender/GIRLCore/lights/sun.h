/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   sun.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - sun.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// sun.cpp*
#include "girl.h"
#include "light.h"
#include "shape.h"
#include "scene.h"

namespace girl
{

// SunLight Declarations
class GIRLCOREDLL  SunLight : public Light {
public:
	// SunLight Public Methods
	SunLight(const Transform &light2world, const float sunscale, const Vector &dir, float turb, float relSize, int ns);
	bool IsDeltaLight() const { return cosThetaMax == 1.0 ? true : false; }
	SWCSpectrum Power(const TsPack *tspack, const Scene *scene) const {
		Point worldCenter;
		float worldRadius;
		scene->WorldBound().BoundingSphere(&worldCenter,
		                                   &worldRadius);
		return SWCSpectrum(tspack, LSPD) * M_PI * worldRadius * worldRadius;
	}
	SWCSpectrum Le(const TsPack *tspack, const RayDifferential &r) const;
	SWCSpectrum Le(const TsPack *tspack, const Scene *scene, const Ray &r,
		const Normal &n, BSDF **bsdf, float *pdf, float *pdfDirect) const;
	SWCSpectrum Sample_L(const TsPack *tspack, const Point &P, float u1, float u2, float u3,
		Vector *wo, float *pdf, VisibilityTester *visibility) const;
	SWCSpectrum Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2,
		float u3, float u4, Ray *ray, float *pdf) const;
	float Pdf(const Point &, const Vector &) const;

	bool Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2, float u3, BSDF **bsdf, float *pdf, SWCSpectrum *Le) const;
	bool Sample_L(const TsPack *tspack, const Scene *scene, const Point &p, const Normal &n, float u1, float u2, float u3, BSDF **bsdf, float *pdf, float *pdfDirect, VisibilityTester *visibility, SWCSpectrum *Le) const;

	static Light *CreateLight(const Transform &light2world,
		const ParamSet &paramSet);

private:
	bool checkPortals(Ray portalRay) const;

	// SunLight Private Data
	Vector sundir;
	// XY Vectors for cone sampling
	Vector x, y;
	float turbidity;
	float thetaS, phiS, V;
	float cosThetaMax, sin2ThetaMax;
	SPD *LSPD;
};

}//namespace girl
