/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   projection.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - projection.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// TODO - Port SPD interfaces

// projection.cpp*
#include "projection.h"
#include "imagereader.h"
#include "mc.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// ProjectionLight Method Definitions
ProjectionLight::
	ProjectionLight(const Transform &light2world,
		const RGBColor &intensity, const string &texname,
		float fov)
	: Light(light2world) {
	lightPos = LightToWorld(Point(0,0,0));
	Intensity = intensity;
	// Create _ProjectionLight_ MIP-map
	int width = 0, height = 0;
	auto_ptr<ImageData> imgdata(ReadImage(texname));
	if (imgdata.get()!=NULL) {
		width=imgdata->getWidth();
		height=imgdata->getHeight();
		projectionMap =imgdata->createMIPMap<RGBColor>();
	}
	else 
		projectionMap = NULL;

	// Initialize _ProjectionLight_ projection matrix
	float aspect = float(width) / float(height);
	if (aspect > 1.f)  {
		screenX0 = -aspect;  screenX1 =  aspect;
		screenY0 = -1.f;     screenY1 =  1.f;
	}
	else {
		screenX0 = -1.f;            screenX1 =  1.f;
		screenY0 = -1.f / aspect;   screenY1 =  1.f / aspect;
	}
	hither = RAY_EPSILON;
	yon = 1e30f;
	lightProjection = Perspective(fov, hither, yon);
	// Compute cosine of cone surrounding projection directions
	float opposite = tanf(Radians(fov) / 2.f);
	float tanDiag = opposite * sqrtf(1.f + 1.f/(aspect*aspect));
	cosTotalWidth = cosf(atanf(tanDiag));
}
ProjectionLight::~ProjectionLight() { delete projectionMap; }
RGBColor ProjectionLight::Projection(const Vector &w) const {
	Vector wl = WorldToLight(w);
	// Discard directions behind projection light
	if (wl.z < hither) return 0.;
	// Project point on to projection plane and compute light
	Point Pl = lightProjection(Point(wl.x, wl.y, wl.z));
	if (Pl.x < screenX0 || Pl.x > screenX1 ||
		Pl.y < screenY0 || Pl.y > screenY1) return 0.;
	if (!projectionMap) return 1;
	float s = (Pl.x - screenX0) / (screenX1 - screenX0);
	float t = (Pl.y - screenY0) / (screenY1 - screenY0);
	return projectionMap->Lookup(s, t);
}
SWCSpectrum ProjectionLight::Sample_L(const TsPack *tspack, const Point &p, float u1, float u2,
		float u3, Vector *wi, float *pdf,
		VisibilityTester *visibility) const {
	*wi = Normalize(lightPos - p);
	*pdf = 1.f;
	visibility->SetSegment(p, lightPos, tspack->time);
	return SWCSpectrum(tspack, Intensity * Projection(-*wi) / DistanceSquared(lightPos, p));
}
SWCSpectrum ProjectionLight::Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2,
		float u3, float u4, Ray *ray, float *pdf) const {
	ray->o = lightPos;
	Vector v = UniformSampleCone(u1, u2, cosTotalWidth);
	ray->d = LightToWorld(v);
	*pdf = UniformConePdf(cosTotalWidth);
	return SWCSpectrum(tspack, Intensity * Projection(ray->d));
}
float ProjectionLight::Pdf(const Point &, const Vector &) const {
	return 0.;
}
Light* ProjectionLight::CreateLight(const Transform &light2world,
		const ParamSet &paramSet) {
	RGBColor I = paramSet.FindOneRGBColor("I", RGBColor(1.0));
	float fov = paramSet.FindOneFloat("fov", 45.);
	string texname = paramSet.FindOneString("mapname", "");
	return new ProjectionLight(light2world, I, texname, fov);
}

static DynamicLoader::RegisterLight<ProjectionLight> r("projection");

