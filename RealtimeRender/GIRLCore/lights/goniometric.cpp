/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   goniometric.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - goniometric.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// TODO - Port SPD interfaces

// goniometric.cpp*
#include "goniometric.h"
#include "imagereader.h"
#include "mc.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// GonioPhotometricLight Method Definitions
GonioPhotometricLight::GonioPhotometricLight(
		const Transform &light2world,
		const RGBColor &intensity, const string &texname)
	: Light(light2world) {
	lightPos = LightToWorld(Point(0,0,0));
	Intensity = intensity;
	// Create _mipmap_ for _GonioPhotometricLight_
	auto_ptr<ImageData> imgdata(ReadImage(texname));
	if (imgdata.get()!=NULL) {
		mipmap = imgdata->createMIPMap<RGBColor>();
	}
	else 
		mipmap = NULL;
}
SWCSpectrum GonioPhotometricLight::Sample_L(const TsPack *tspack, const Point &P, float u1, float u2,
		float u3, Vector *wo, float *pdf,
		VisibilityTester *visibility) const {
	*wo = Normalize(lightPos - P);
	*pdf = 1.f;
	visibility->SetSegment(P, lightPos, tspack->time);
	return SWCSpectrum(tspack, Intensity * Scale(-*wo) / DistanceSquared(lightPos, P));
}
SWCSpectrum GonioPhotometricLight::Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2,
		float u3, float u4, Ray *ray, float *pdf) const {
	ray->o = lightPos;
	ray->d = UniformSampleSphere(u1, u2);
	*pdf = UniformSpherePdf();
	return SWCSpectrum(tspack, Intensity * Scale(ray->d));
}
float GonioPhotometricLight::Pdf(const Point &, const Vector &) const {
	return 0.;
}
Light* GonioPhotometricLight::CreateLight(const Transform &light2world,
		const ParamSet &paramSet) {
	RGBColor I = paramSet.FindOneRGBColor("I", RGBColor(1.0));
	string texname = paramSet.FindOneString("mapname", "");
	return new GonioPhotometricLight(light2world, I, texname);
}

static DynamicLoader::RegisterLight<GonioPhotometricLight> r("goniometric");

