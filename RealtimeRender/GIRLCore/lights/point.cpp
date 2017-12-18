/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   point.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - point.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// point.cpp*
#include "point.h"
#include "mc.h"
#include "spd.h"
#include "reflection/bxdf.h"
#include "reflection/bxdf/lambertian.h"
#include "rgbillum.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// PointLight Method Definitions
PointLight::PointLight(const Transform &light2world,
		const RGBColor &intensity, float gain)
	: Light(light2world) {
	lightPos = LightToWorld(Point(0,0,0));
	// Create SPD
	LSPD = new RGBIllumSPD(intensity);
	LSPD->Scale(gain);
}
PointLight::~PointLight()
{
	delete LSPD;
}
SWCSpectrum PointLight::Sample_L(const TsPack *tspack, const Point &p, float u1,
		float u2, float u3, Vector *wi, float *pdf,
		VisibilityTester *visibility) const {
	*pdf = 1.f;
	*wi = Normalize(lightPos - p);
	visibility->SetSegment(p, lightPos, tspack->time);
	return SWCSpectrum(tspack, LSPD) / DistanceSquared(lightPos, p);
}
float PointLight::Pdf(const Point &, const Vector &) const {
	return 0.;
}
SWCSpectrum PointLight::Sample_L(const TsPack *tspack, const Scene *scene, float u1,
		float u2, float u3, float u4,
		Ray *ray, float *pdf) const {
	ray->o = lightPos;
	ray->d = UniformSampleSphere(u1, u2);
	*pdf = UniformSpherePdf();
	return SWCSpectrum(tspack, LSPD);
}
bool PointLight::Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2, float u3, BSDF **bsdf, float *pdf, SWCSpectrum *Le) const
{
	Normal ns = Normal(UniformSampleSphere(u1, u2));
	Vector dpdu, dpdv;
	CoordinateSystem(Vector(ns), &dpdu, &dpdv);
	DifferentialGeometry dg(lightPos, ns, dpdu, dpdv, Vector(0, 0, 0), Vector(0, 0, 0), 0, 0, NULL);
	*bsdf = BSDF_ALLOC(tspack, BSDF)(dg, ns);
	(*bsdf)->Add(BSDF_ALLOC(tspack, Lambertian)(SWCSpectrum(1.f)));
	*pdf = .25f * INV_PI;
	*Le = SWCSpectrum(tspack, LSPD);
	return true;
}
bool PointLight::Sample_L(const TsPack *tspack, const Scene *scene, const Point &p, const Normal &n,
	float u1, float u2, float u3, BSDF **bsdf, float *pdf, float *pdfDirect,
	VisibilityTester *visibility, SWCSpectrum *Le) const
{
	const Vector w(p - lightPos);
	*pdfDirect = 1.f / w.LengthSquared();
	Normal ns = Normal(w * *pdfDirect);
	*pdf = .25f * INV_PI;
	Vector dpdu, dpdv;
	CoordinateSystem(Vector(ns), &dpdu, &dpdv);
	DifferentialGeometry dg(lightPos, ns, dpdu, dpdv, Vector(0, 0, 0), Vector(0, 0, 0), 0, 0, NULL);
	*bsdf = BSDF_ALLOC(tspack, BSDF)(dg, ns);
	(*bsdf)->Add(BSDF_ALLOC(tspack, Lambertian)(SWCSpectrum(1.f)));
	visibility->SetSegment(p, lightPos, tspack->time);
	*Le = SWCSpectrum(tspack, LSPD);
	return true;
}
SWCSpectrum PointLight::Le(const TsPack *tspack, const Scene *scene, const Ray &r,
	const Normal &n, BSDF **bsdf, float *pdf, float *pdfDirect) const
{
	return SWCSpectrum(0.f);
}
Light* PointLight::CreateLight(const Transform &light2world,
		const ParamSet &paramSet) {
	RGBColor intensity = paramSet.FindOneRGBColor("I", RGBColor(1.0));
	float g = paramSet.FindOneFloat("gain", 1.f);
	Point P = paramSet.FindOnePoint("from", Point(0,0,0));
	Transform l2w = Translate(Vector(P.x, P.y, P.z)) * light2world;
	return new PointLight(l2w, intensity, g);
}

static DynamicLoader::RegisterLight<PointLight> r("point");

