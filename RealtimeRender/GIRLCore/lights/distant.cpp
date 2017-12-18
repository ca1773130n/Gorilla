/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   distant.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - distant.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// distant.cpp*
#include "distant.h"
#include "mc.h"
#include "spd.h"
#include "rgbillum.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// DistantLight Method Definitions
DistantLight::DistantLight(const Transform &light2world,
		const RGBColor &radiance, float gain, const Vector &dir)
	: Light(light2world) {
	lightDir = Normalize(LightToWorld(dir));
	// Create SPD
	LSPD = new RGBIllumSPD(radiance);
	LSPD->Scale(gain);
}
DistantLight::~DistantLight()
{
	delete LSPD;
}
SWCSpectrum DistantLight::Sample_L(const TsPack *tspack, const Point &p, float u1, float u2, float u3,
		Vector *wi, float *pdf, VisibilityTester *visibility) const {
	*pdf = 1.f;
	*wi = lightDir;
	visibility->SetRay(p, *wi, tspack->time);
	return SWCSpectrum(tspack, LSPD);
}
float DistantLight::Pdf(const Point &, const Vector &) const {
	return 0.;
}
SWCSpectrum DistantLight::Sample_L(const TsPack *tspack, const Scene *scene,
		float u1, float u2, float u3, float u4,
		Ray *ray, float *pdf) const {
	// Choose point on disk oriented toward infinite light direction
	Point worldCenter;
	float worldRadius;
	scene->WorldBound().BoundingSphere(&worldCenter,
	                                   &worldRadius);
	Vector v1, v2;
	CoordinateSystem(lightDir, &v1, &v2);
	float d1, d2;
	ConcentricSampleDisk(u1, u2, &d1, &d2);
	Point Pdisk =
		worldCenter + worldRadius * (d1 * v1 + d2 * v2);
	// Set ray origin and direction for infinite light ray
	ray->o = Pdisk + worldRadius * lightDir;
	ray->d = -lightDir;
	*pdf = 1.f / (M_PI * worldRadius * worldRadius);
	return SWCSpectrum(tspack, LSPD);
}
Light* DistantLight::CreateLight(const Transform &light2world,
		const ParamSet &paramSet) {
	RGBColor L = paramSet.FindOneRGBColor("L", RGBColor(1.0));
	float g = paramSet.FindOneFloat("gain", 1.f);
	Point from = paramSet.FindOnePoint("from", Point(0,0,0));
	Point to = paramSet.FindOnePoint("to", Point(0,0,1));
	Vector dir = from-to;
	return new DistantLight(light2world, L, g, dir);
}

static DynamicLoader::RegisterLight<DistantLight> r("distant");

