/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   spot.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - spot.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// spot.cpp*
#include "spot.h"
#include "mc.h"
#include "spd.h"
#include "rgbillum.h"
#include "paramset.h"
#include "reflection/bxdf.h"
#include "dynload.h"

using namespace girl;

static float LocalFalloff(const Vector &w, float cosTotalWidth, float cosFalloffStart)
{
	if (CosTheta(w) < cosTotalWidth)
		return 0.f;
 	if (CosTheta(w) > cosFalloffStart)
		return 1.f;
	// Compute falloff inside spotlight cone
	const float delta = (CosTheta(w) - cosTotalWidth) /
		(cosFalloffStart - cosTotalWidth);
	return powf(delta, 4);
}
class GIRLCOREDLL SpotBxDF : public BxDF
{
public:
	SpotBxDF(float width, float fall) : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), cosTotalWidth(width), cosFalloffStart(fall) {}
	void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const {
		*f += LocalFalloff(wi, cosTotalWidth, cosFalloffStart);
	}
	bool Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi, float u1, float u2, SWCSpectrum *const f,float *pdf, float *pdfBack = NULL, bool reverse = false) const
	{
		*wi = UniformSampleCone(u1, u2, cosTotalWidth);
		*pdf = UniformConePdf(cosTotalWidth);
		if (pdfBack)
			*pdfBack = Pdf(*wi, wo);
		*f = LocalFalloff(*wi, cosTotalWidth, cosFalloffStart);
		return true;
	}
	float Pdf(const Vector &wi, const Vector &wo) const
	{
		if (CosTheta(wo) < cosTotalWidth)
			return 0.f;
		else
			return UniformConePdf(cosTotalWidth);
	}
private:
	float cosTotalWidth, cosFalloffStart;
};

// SpotLight Method Definitions
SpotLight::SpotLight(const Transform &light2world,
		const RGBColor &intensity, float gain, float width, float fall)
	: Light(light2world) {
	lightPos = LightToWorld(Point(0,0,0));

	// Create SPD
	LSPD = new RGBIllumSPD(intensity);
	LSPD->Scale(gain);

	cosTotalWidth = cosf(Radians(width));
	cosFalloffStart = cosf(Radians(fall));
}
SpotLight::~SpotLight()
{
	delete LSPD;
}
float SpotLight::Falloff(const Vector &w) const {
	return LocalFalloff(Normalize(WorldToLight(w)), cosTotalWidth, cosFalloffStart);
}
SWCSpectrum SpotLight::Sample_L(const TsPack *tspack, const Point &p, float u1, float u2, float u3,
		Vector *wi, float *pdf, VisibilityTester *visibility) const {
	*pdf = 1.f;
	*wi = Normalize(lightPos - p);
	visibility->SetSegment(p, lightPos, tspack->time);
	return SWCSpectrum(tspack, LSPD) * Falloff(-*wi) /
		DistanceSquared(lightPos, p);
}
float SpotLight::Pdf(const Point &, const Vector &) const {
	return 0.;
}
SWCSpectrum SpotLight::Sample_L(const TsPack *tspack, const Scene *scene, float u1,
		float u2, float u3, float u4,
		Ray *ray, float *pdf) const {
	ray->o = lightPos;
	Vector v = UniformSampleCone(u1, u2, cosTotalWidth);
	ray->d = LightToWorld(v);
	*pdf = UniformConePdf(cosTotalWidth);
	return SWCSpectrum(tspack, LSPD) * Falloff(ray->d);
}
bool SpotLight::Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2, float u3, BSDF **bsdf, float *pdf, SWCSpectrum *Le) const
{
	Normal ns = LightToWorld(Normal(0, 0, 1));
	Vector dpdu, dpdv;
	CoordinateSystem(Vector(ns), &dpdu, &dpdv);
	DifferentialGeometry dg(lightPos, ns, dpdu, dpdv, Vector(0, 0, 0), Vector(0, 0, 0), 0, 0, NULL);
	*bsdf = BSDF_ALLOC(tspack, BSDF)(dg, ns);
	(*bsdf)->Add(BSDF_ALLOC(tspack, SpotBxDF)(cosTotalWidth, cosFalloffStart));
	*pdf = 1.f;
	*Le = SWCSpectrum(tspack, LSPD);
	return true;
}
bool SpotLight::Sample_L(const TsPack *tspack, const Scene *scene, const Point &p, const Normal &n,
	float u1, float u2, float u3, BSDF **bsdf, float *pdf, float *pdfDirect,
	VisibilityTester *visibility, SWCSpectrum *Le) const
{
	const Vector w(p - lightPos);
	*pdfDirect = 1.f / w.LengthSquared();
	Normal ns = LightToWorld(Normal(0, 0, 1));
	*pdf = 1.f;
	Vector dpdu, dpdv;
	CoordinateSystem(Vector(ns), &dpdu, &dpdv);
	DifferentialGeometry dg(lightPos, ns, dpdu, dpdv, Vector(0, 0, 0), Vector(0, 0, 0), 0, 0, NULL);
	*bsdf = BSDF_ALLOC(tspack, BSDF)(dg, ns);
	(*bsdf)->Add(BSDF_ALLOC(tspack, SpotBxDF)(cosTotalWidth, cosFalloffStart));
	visibility->SetSegment(p, lightPos, tspack->time);
	*Le = SWCSpectrum(tspack, LSPD);
	return true;
}
SWCSpectrum SpotLight::Le(const TsPack *tspack, const Scene *scene, const Ray &r,
	const Normal &n, BSDF **bsdf, float *pdf, float *pdfDirect) const
{
	return SWCSpectrum(0.f);
}
Light* SpotLight::CreateLight(const Transform &l2w, const ParamSet &paramSet) {
	RGBColor I = paramSet.FindOneRGBColor("I", RGBColor(1.0));
	float g = paramSet.FindOneFloat("gain", 1.f);
	float coneangle = paramSet.FindOneFloat("coneangle", 30.);
	float conedelta = paramSet.FindOneFloat("conedeltaangle", 5.);
	// Compute spotlight world to light transformation
	Point from = paramSet.FindOnePoint("from", Point(0,0,0));
	Point to = paramSet.FindOnePoint("to", Point(0,0,1));
	Vector dir = Normalize(to - from);
	Vector du, dv;
	CoordinateSystem(dir, &du, &dv);
	boost::shared_ptr<Matrix4x4> o (new Matrix4x4( du.x,  du.y,  du.z, 0.,
	                                 dv.x,  dv.y,  dv.z, 0.,
	                                dir.x, dir.y, dir.z, 0.,
	                                    0,     0,     0, 1.));
	Transform dirToZ = Transform(o);
	Transform light2world =
	l2w *
	Translate(Vector(from.x, from.y, from.z)) *
	dirToZ.GetInverse();
	return new SpotLight(light2world, I, g, coneangle,
		coneangle-conedelta);
}

static DynamicLoader::RegisterLight<SpotLight> r("spot");

