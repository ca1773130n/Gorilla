/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   directlighting.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - directlighting.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunications Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// directlighting.cpp*
#include "directlighting.h"
#include "bxdf.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// DirectLightingIntegrator Method Definitions
DirectLightingIntegrator::DirectLightingIntegrator(LightStrategy st, int md) {
	maxDepth = md;
	lightStrategy = st;
}

void DirectLightingIntegrator::RequestSamples(Sample *sample, const Scene *scene) {
	if (lightStrategy == SAMPLE_AUTOMATIC) {
		if (scene->lights.size() > 5)
			lightStrategy = SAMPLE_ONE_UNIFORM;
		else
			lightStrategy = SAMPLE_ALL_UNIFORM;
	}

	vector<u_int> structure;
	// Dade - allocate and request samples for light sampling
	structure.push_back(2);	// light position sample
	structure.push_back(1);	// light number sample
	structure.push_back(2);	// bsdf direction sample for light
	structure.push_back(1);	// bsdf component sample for light

	sampleOffset = sample->AddxD(structure, maxDepth + 1);
}

SWCSpectrum DirectLightingIntegrator::LiInternal(const TsPack *tspack, const Scene *scene,
		const RayDifferential &ray, const Sample *sample,
		float *alpha, int rayDepth) const {
	Intersection isect;
	SWCSpectrum L(0.);
	if (alpha) *alpha = 1.;
    const float time = ray.time; // save time for motion blur

	if (scene->Intersect(ray, &isect)) {
		// Dade - collect samples
		float *sampleData = sample->sampler->GetLazyValues(const_cast<Sample *>(sample), sampleOffset, rayDepth);
		float *lightSample = &sampleData[0];
		float *lightNum = &sampleData[2];
		float *bsdfSample = &sampleData[3];
		float *bsdfComponent = &sampleData[5];

		// Evaluate BSDF at hit point
		BSDF *bsdf = isect.GetBSDF(tspack, ray, fabsf(2.f * bsdfComponent[0] - 1.f));
		Vector wo = -ray.d;
		const Point &p = bsdf->dgShading.p;
		const Normal &n = bsdf->dgShading.nn;

		// Compute emitted light if ray hit an area light source
		L += isect.Le(tspack, wo);

		// Compute direct lighting
		if (scene->lights.size() > 0) {
			// Apply direct lighting strategy
			switch (lightStrategy) {
				case SAMPLE_ALL_UNIFORM:
					L += UniformSampleAllLights(tspack, scene, p, n,
						wo, bsdf, sample,
						lightSample, lightNum, bsdfSample, bsdfComponent);
					break;
				case SAMPLE_ONE_UNIFORM:
					L += UniformSampleOneLight(tspack, scene, p, n,
						wo, bsdf, sample,
						lightSample, lightNum, bsdfSample, bsdfComponent);
					break;
				default:
					break;
			}
		}

		if (rayDepth < maxDepth) {
			Vector wi;
			// Trace rays for specular reflection and refraction
			float pdf;
			SWCSpectrum f;
			if (bsdf->Sample_f(tspack, wo, &wi, .5f, .5f, .5f, &f, &pdf, BxDFType(BSDF_REFLECTION | BSDF_SPECULAR))) {
				// Compute ray differential _rd_ for specular reflection
				RayDifferential rd(p, wi);
				rd.time = time;
				rd.hasDifferentials = true;
				rd.rx.o = p + isect.dg.dpdx;
				rd.ry.o = p + isect.dg.dpdy;
				// Compute differential reflected directions
				Normal dndx = bsdf->dgShading.dndu * bsdf->dgShading.dudx +
					bsdf->dgShading.dndv * bsdf->dgShading.dvdx;
				Normal dndy = bsdf->dgShading.dndu * bsdf->dgShading.dudy +
					bsdf->dgShading.dndv * bsdf->dgShading.dvdy;
				Vector dwodx = -ray.rx.d - wo, dwody = -ray.ry.d - wo;
				float dDNdx = Dot(dwodx, n) + Dot(wo, dndx);
				float dDNdy = Dot(dwody, n) + Dot(wo, dndy);
				rd.rx.d = wi -
				          dwodx + 2 * Vector(Dot(wo, n) * dndx +
						  dDNdx * n);
				rd.ry.d = wi -
				          dwody + 2 * Vector(Dot(wo, n) * dndy +
						  dDNdy * n);
				//L += scene->Li(rd, sample) * f * AbsDot(wi, n);
				L += LiInternal(tspack, scene, rd, sample, alpha, rayDepth + 1) * f * AbsDot(wi, n);
			}

			if (bsdf->Sample_f(tspack, wo, &wi, .5f, .5f, .5f, &f, &pdf, BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR))) {
				// Compute ray differential _rd_ for specular transmission
				RayDifferential rd(p, wi);
				rd.time = time;
				rd.hasDifferentials = true;
				rd.rx.o = p + isect.dg.dpdx;
				rd.ry.o = p + isect.dg.dpdy;
				
				float eta = bsdf->eta;
				Vector w = -wo;
				if (Dot(wo, n) < 0) eta = 1.f / eta;
				
				Normal dndx = bsdf->dgShading.dndu * bsdf->dgShading.dudx + bsdf->dgShading.dndv * bsdf->dgShading.dvdx;
				Normal dndy = bsdf->dgShading.dndu * bsdf->dgShading.dudy + bsdf->dgShading.dndv * bsdf->dgShading.dvdy;
				
				Vector dwodx = -ray.rx.d - wo, dwody = -ray.ry.d - wo;
				float dDNdx = Dot(dwodx, n) + Dot(wo, dndx);
				float dDNdy = Dot(dwody, n) + Dot(wo, dndy);
				
				float mu = eta * Dot(w, n) - Dot(wi, n);
				float dmudx = (eta - (eta*eta*Dot(w,n))/Dot(wi, n)) * dDNdx;
				float dmudy = (eta - (eta*eta*Dot(w,n))/Dot(wi, n)) * dDNdy;
				
				rd.rx.d = wi + eta * dwodx - Vector(mu * dndx + dmudx * n);
				rd.ry.d = wi + eta * dwody - Vector(mu * dndy + dmudy * n);
				//L += scene->Li(rd, sample) * f * AbsDot(wi, n);
				L += LiInternal(tspack, scene, rd, sample, alpha, rayDepth + 1) * f * AbsDot(wi, n);
			}
		}
	} else {
		// Handle ray with no intersection
		for (u_int i = 0; i < scene->lights.size(); ++i)
			L += scene->lights[i]->Le(tspack, ray);
		if (alpha && L.Black()) *alpha = 0.;
	}

	SWCSpectrum Tr(1.f);
	scene->volumeIntegrator->Transmittance(tspack, scene, ray, sample, alpha, &Tr);
	SWCSpectrum VLi = scene->volumeIntegrator->Li(tspack, scene, ray, sample, alpha);

	return L * Tr + VLi;
}

SWCSpectrum DirectLightingIntegrator::Li(const TsPack *tspack, const Scene *scene,
		const RayDifferential &ray, const Sample *sample,
		float *alpha) const {
	SampleGuard guard(sample->sampler, sample);

	sample->AddContribution(sample->imageX, sample->imageY,
		LiInternal(tspack, scene, ray, sample, alpha, 0).ToXYZ(tspack),
		alpha ? *alpha : 1.f);

	return SWCSpectrum(1.f);
}

SurfaceIntegrator* DirectLightingIntegrator::CreateSurfaceIntegrator(const ParamSet &params) {
	int maxDepth = params.FindOneInt("maxdepth", 5);

	LightStrategy estrategy;
	string st = params.FindOneString("strategy", "auto");
	if (st == "one") estrategy = SAMPLE_ONE_UNIFORM;
	else if (st == "all") estrategy = SAMPLE_ALL_UNIFORM;
	else if (st == "auto") estrategy = SAMPLE_AUTOMATIC;
	else {
		std::stringstream ss;
		ss<<"Strategy  '"<<st<<"' for direct lighting unknown. Using \"auto\".";
		girlError(GIRL_BADTOKEN,GIRL_WARNING,ss.str().c_str());
		estrategy = SAMPLE_AUTOMATIC;
	}

	return new DirectLightingIntegrator(estrategy, maxDepth);
}

static DynamicLoader::RegisterSurfaceIntegrator<DirectLightingIntegrator> r("directlighting");
