/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   infinitesample.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - infinitesample.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// TODO - Port SPD interfaces

// infinitesample.cpp*
#include "infinitesample.h"
#include "imagereader.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// InfiniteAreaLightIS Method Definitions
InfiniteAreaLightIS::~InfiniteAreaLightIS() {
	delete radianceMap;
}
InfiniteAreaLightIS
	::InfiniteAreaLightIS(const Transform &light2world,
				const RGBColor &L, int ns,
				const string &texmap)
	: Light(light2world, ns)
{
	int width = 0, height = 0;
	Lbase = L;
	radianceMap = NULL;
	if (texmap != "") {
		auto_ptr<ImageData> imgdata(ReadImage(texmap));
		if(imgdata.get() !=NULL)
		{
			width=imgdata->getWidth();
			height=imgdata->getHeight();
			radianceMap = imgdata->createMIPMap<RGBColor>();
		}
	}
	if (radianceMap == NULL)
		return;
	// Compute scalar-valued image from environment map
	float filter = 1.f / max(width, height);
	int nu = width, nv = height;
	float *img = new float[width*height];
	for (int x = 0; x < nu; ++x) {
		float xp = (float)x / (float)nu;
		for (int y = 0; y < nv; ++y) {
			float yp = (float)y / (float)nv;
			img[y+x*nv] = radianceMap->Lookup(xp, yp, filter).y();
		}
	}
	// Initialize sampling PDFs for infinite area light
	float *func = (float *)alloca(max(nu, nv) * sizeof(float));
	float *sinVals = (float *)alloca(nv * sizeof(float));
	for (int i = 0; i < nv; ++i)
		sinVals[i] = sin(M_PI * float(i+.5)/float(nv));
	vDistribs = new Distribution1D *[nu];
	for (int u = 0; u < nu; ++u) {
		// Compute sampling distribution for column _u_
		for (int v = 0; v < nv; ++v)
			func[v] = img[u*nv+v] *= sinVals[v];
		vDistribs[u] = new Distribution1D(func, nv);
	}
	// Compute sampling distribution for columns of image
	for (int u = 0; u < nu; ++u)
		func[u] = vDistribs[u]->funcInt;
	uDistrib = new Distribution1D(func, nu);
	delete[] img;
}
SWCSpectrum
	InfiniteAreaLightIS::Le(const TsPack *tspack, const RayDifferential &r) const {
	Vector w = r.d;
	// Compute infinite light radiance for direction
	RGBColor L = Lbase;
	if (radianceMap != NULL) {
		Vector wh = Normalize(WorldToLight(w));
		float s = SphericalPhi(wh) * INV_TWOPI;
		float t = SphericalTheta(wh) * INV_PI;
		L *= radianceMap->Lookup(s, t);
	}
	return SWCSpectrum(tspack, L);
}
SWCSpectrum InfiniteAreaLightIS::Sample_L(const TsPack *tspack, const Point &p, float u1,
		float u2, float u3, Vector *wi, float *pdf,
		VisibilityTester *visibility) const {
	// Find floating-point $(u,v)$ sample coordinates
	float pdfs[2];
	float fu = uDistrib->Sample(u1, &pdfs[0]);
	int u = Clamp(Float2Int(fu), 0, uDistrib->count-1);
	float fv = vDistribs[u]->Sample(u2, &pdfs[1]);
	// Convert sample point to direction on the unit sphere
	float theta = fv * vDistribs[u]->invCount * M_PI;
	float phi = fu * uDistrib->invCount * 2.f * M_PI;
	float costheta = cos(theta), sintheta = sin(theta);
	float sinphi = sin(phi), cosphi = cos(phi);
	*wi = LightToWorld(Vector(sintheta * cosphi, sintheta * sinphi,
	                          costheta));
	// Compute PDF for sampled direction
	*pdf = (pdfs[0] * pdfs[1]) / (2. * M_PI * M_PI * sintheta);
	// Return radiance value for direction
	visibility->SetRay(p, *wi, tspack->time);
	return SWCSpectrum(tspack, Lbase * radianceMap->Lookup(fu * uDistrib->invCount,
		fv * vDistribs[u]->invCount));
}
float InfiniteAreaLightIS::Pdf(const Point &,
		const Vector &w) const {
	Vector wi = WorldToLight(w);
	float theta = SphericalTheta(wi), phi = SphericalPhi(wi);
	int u = Clamp(Float2Int(phi * INV_TWOPI * uDistrib->count),
                  0, uDistrib->count-1);
	int v = Clamp(Float2Int(theta * INV_PI * vDistribs[u]->count),
                  0, vDistribs[u]->count-1);
	return (uDistrib->func[u] * vDistribs[u]->func[v]) /
           (uDistrib->funcInt * vDistribs[u]->funcInt) *
           1.f / (2.f * M_PI * M_PI * sin(theta));
}
SWCSpectrum InfiniteAreaLightIS::Sample_L(const TsPack *tspack, const Scene *scene,
		float u1, float u2, float u3, float u4,
		Ray *ray, float *pdf) const {
	// Choose two points _p1_ and _p2_ on scene bounding sphere
	Point worldCenter;
	float worldRadius;
	scene->WorldBound().BoundingSphere(&worldCenter,
		&worldRadius);
	worldRadius *= 1.01f;
	Point p1 = worldCenter + worldRadius *
		UniformSampleSphere(u1, u2);
	Point p2 = worldCenter + worldRadius *
		UniformSampleSphere(u3, u4);
	// Construct ray between _p1_ and _p2_
	ray->o = p1;
	ray->d = Normalize(p2-p1);
	// Compute _InfiniteAreaLightIS_ ray weight
	Vector to_center = Normalize(worldCenter - p1);
	float costheta = AbsDot(to_center,ray->d);
	*pdf =
		costheta / ((4.f * M_PI * worldRadius * worldRadius));
	return Le(tspack, RayDifferential(ray->o, -ray->d));
}
Light* InfiniteAreaLightIS::CreateLight(const Transform &light2world,
		const ParamSet &paramSet) {
	RGBColor L = paramSet.FindOneRGBColor("L", RGBColor(1.0));
	string texmap = paramSet.FindOneString("mapname", "");
	int nSamples = paramSet.FindOneInt("nsamples", 1);

	return new InfiniteAreaLightIS(light2world, L, nSamples, texmap);
}

static DynamicLoader::RegisterLight<InfiniteAreaLightIS> r("infinitesample");

