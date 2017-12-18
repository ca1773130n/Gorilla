/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   infinitesample.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - infinitesample.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// infinitesample.cpp*
#include "girl.h"
#include "light.h"
#include "mc.h"
#include "texture.h"
#include "shape.h"
#include "scene.h"
#include "mipmap.h"

namespace girl
{

// Utility Classes and Functions
struct Distribution1D {
	// Distribution1D Methods
	Distribution1D(float *f, int n) {
		func = new float[n];
		cdf = new float[n+1];
		count = n;
		memcpy(func, f, n*sizeof(float));
		ComputeStep1dCDF(func, n, &funcInt, cdf);
		invFuncInt = 1.f / funcInt;
		invCount = 1.f / count;
	}
	float Sample(float u, float *pdf) {
		// Find surrounding cdf segments
		float *ptr = std::lower_bound(cdf, cdf+count+1, u);
		int offset = (int) (ptr-cdf-1);
		// Return offset along current cdf segment
		u = (u - cdf[offset]) / (cdf[offset+1] - cdf[offset]);
		*pdf = func[offset] * invFuncInt;
		return offset + u;
	}
	// Distribution1D Data
	float *func, *cdf;
	float funcInt, invFuncInt, invCount;
	int count;
};
// InfiniteAreaLightIS Definitions
class GIRLCOREDLL  InfiniteAreaLightIS : public Light {
public:
	// InfiniteAreaLightIS Public Methods
	InfiniteAreaLightIS(const Transform &light2world,	const RGBColor &power, int ns,
			  const string &texmap);
	~InfiniteAreaLightIS();
	SWCSpectrum Power(const TsPack *tspack, const Scene *scene) const {
		Point worldCenter;
		float worldRadius;
		scene->WorldBound().BoundingSphere(&worldCenter,
			&worldRadius);
		return SWCSpectrum(tspack, Lbase * radianceMap->Lookup(.5f, .5f, .5f) *
			M_PI * worldRadius * worldRadius);
	}
	bool IsDeltaLight() const { return false; }
	SWCSpectrum Le(const TsPack *tspack, const RayDifferential &r) const;
	SWCSpectrum Sample_L(const TsPack *tspack, const Point &p, float u1, float u2, float u3,
		Vector *wi, float *pdf, VisibilityTester *visibility) const;
	SWCSpectrum Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2,
			float u3, float u4, Ray *ray, float *pdf) const;
	float Pdf(const Point &, const Vector &) const;

	static Light *CreateLight(const Transform &light2world,
		const ParamSet &paramSet);

private:
	// InfiniteAreaLightIS Private Data
	RGBColor Lbase;
	MIPMap<RGBColor> *radianceMap;
	Distribution1D *uDistrib, **vDistribs;
};

}

