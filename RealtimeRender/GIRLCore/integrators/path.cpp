/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   path.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - path.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// path.cpp*
#include "path.h"
#include "bxdf.h"
#include "light.h"
#include "camera.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// PathIntegrator Method Definitions
void PathIntegrator::RequestSamples(Sample *sample, const Scene *scene)
{
	if (lightStrategy == SAMPLE_AUTOMATIC) {
		if (scene->sampler->IsMutating() || scene->lights.size() > 5)
			lightStrategy = SAMPLE_ONE_UNIFORM;
		else
			lightStrategy = SAMPLE_ALL_UNIFORM;
	}

	vector<u_int> structure;
	structure.push_back(2);	// light position sample
	structure.push_back(1);	// light number sample
	structure.push_back(2);	// bsdf direction sample for light
	structure.push_back(1);	// bsdf component sample for light
	structure.push_back(2);	// bsdf direction sample for path
	structure.push_back(1);	// bsdf component sample for path
	if (rrStrategy != RR_NONE)
		structure.push_back(1);	// continue sample
	sampleOffset = sample->AddxD(structure, maxDepth + 1);
}
void PathIntegrator::Preprocess(const TsPack *tspack, const Scene *scene)
{
	// Prepare image buffers
	BufferType type = BUF_TYPE_PER_PIXEL;
	scene->sampler->GetBufferType(&type);
	bufferId = scene->camera->film->RequestBuffer(type, BUF_FRAMEBUFFER, "eye");
}

SWCSpectrum PathIntegrator::Li(const TsPack *tspack, const Scene *scene,
		const RayDifferential &r, const Sample *sample,
		float *alpha) const
{
	SampleGuard guard(sample->sampler, sample);
	float nrContribs = 0.f;
	// Declare common path integration variables
	RayDifferential ray(r);
	SWCSpectrum pathThroughput(1.0f);
	SWCSpectrum L(0.0f);
	float V = 0.f, VContrib = .1f;
	bool specularBounce = true, specular = true;
	if (alpha) *alpha = 1.;
	for (int pathLength = 0; ; ++pathLength) {
		// Find next vertex of path
		Intersection isect;
		if (!scene->Intersect(ray, &isect)) {
			if (pathLength == 0) {
				// Dade - now I know ray.maxt and I can call volumeIntegrator
				L = scene->volumeIntegrator->Li(tspack, scene, ray, sample, alpha);
				if (!L.Black())
					V += L.filter(tspack) * VContrib;
				pathThroughput = 1.f;
				scene->volumeIntegrator->Transmittance(tspack, scene, ray, sample, alpha, &pathThroughput);
			}

			// Stop path sampling since no intersection was found
			// Possibly add emitted light
			// NOTE - Added by radiance - adds horizon in render & reflections
			if (specularBounce) {
				SWCSpectrum Le(0.f);
				for (u_int i = 0; i < scene->lights.size(); ++i)
					Le += scene->lights[i]->Le(tspack, ray);
				Le *= pathThroughput;
				if (!Le.Black()) {
					L += Le;
					V += Le.filter(tspack) * VContrib;
					++nrContribs;
				}
			}
			// Set alpha channel
			if (pathLength == 0 && alpha && L.Black())
				*alpha = 0.;
			break;
		}
		if (pathLength == 0)
			r.maxt = ray.maxt;

		SWCSpectrum Lv(scene->volumeIntegrator->Li(tspack, scene, ray, sample, alpha));
		Lv *= pathThroughput;
		if (!Lv.Black()) {
			L += Lv;
			V += Lv.filter(tspack) * VContrib;
			++nrContribs;
		}
		scene->volumeIntegrator->Transmittance(tspack, scene, ray, sample, alpha, &pathThroughput);

		// Possibly add emitted light at path vertex
		Vector wo(-ray.d);
		if (specularBounce) {
			SWCSpectrum Le(isect.Le(tspack, wo));
			Le *= pathThroughput;
			if (!Le.Black()) {
				L += Le;
				V += Le.filter(tspack) * VContrib;
				++nrContribs;
			}
		}
		if (pathLength == maxDepth)
			break;
		// Evaluate BSDF at hit point
		float *data = sample->sampler->GetLazyValues(const_cast<Sample *>(sample), sampleOffset, pathLength);
		BSDF *bsdf = isect.GetBSDF(tspack, ray, fabsf(2.f * data[5] - 1.f));
		// Sample illumination from lights to find path contribution
		const Point &p = bsdf->dgShading.p;
		const Normal &n = bsdf->dgShading.nn;

		SWCSpectrum Ll;
		switch (lightStrategy) {
			case SAMPLE_ALL_UNIFORM:
				Ll = UniformSampleAllLights(tspack, scene, p, n,
					wo, bsdf, sample,
					data, data + 2, data + 3, data + 5);
				break;
			case SAMPLE_ONE_UNIFORM:
				Ll = UniformSampleOneLight(tspack, scene, p, n,
					wo, bsdf, sample,
					data, data + 2, data + 3, data + 5);
				break;
			default:
				Ll = 0.f;
		}
		Ll *= pathThroughput;
		if (!Ll.Black()) {
			L += Ll;
			V += Ll.filter(tspack) * VContrib;
			++nrContribs;
		}

		// Sample BSDF to get new path direction
		Vector wi;
		float pdf;
		BxDFType flags;
		SWCSpectrum f;
		if (!bsdf->Sample_f(tspack, wo, &wi, data[6], data[7], data[8], &f, &pdf, BSDF_ALL, &flags))
			break;

		const float dp = AbsDot(wi, n) / pdf;

		// Possibly terminate the path
		if (pathLength > 3) {
			if (rrStrategy == RR_EFFICIENCY) { // use efficiency optimized RR
				const float q = min<float>(1.f, f.filter(tspack) * dp);
				if (q < data[9])
					break;
				// increase path contribution
				pathThroughput /= q;
			} else if (rrStrategy == RR_PROBABILITY) { // use normal/probability RR
				if (continueProbability < data[9])
					break;
				// increase path contribution
				pathThroughput /= continueProbability;
			}
		}

		specularBounce = (flags & BSDF_SPECULAR) != 0;
		specular = specular && specularBounce;
		pathThroughput *= f;
		pathThroughput *= dp;
		if (!specular)
			VContrib += dp;

		ray = RayDifferential(p, wi);
		ray.time = r.time;
	}
	if (!L.Black())
		V /= L.filter(tspack);
	sample->AddContribution(sample->imageX, sample->imageY,
		L.ToXYZ(tspack), alpha ? *alpha : 1.f, V, bufferId);

	return nrContribs;
}
SurfaceIntegrator* PathIntegrator::CreateSurfaceIntegrator(const ParamSet &params)
{
	// general
	int maxDepth = params.FindOneInt("maxdepth", 16);
	float RRcontinueProb = params.FindOneFloat("rrcontinueprob", .65f);			// continueprobability for plain RR (0.0-1.0)
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
	RRStrategy rstrategy;
	string rst = params.FindOneString("rrstrategy", "efficiency");
	if (rst == "efficiency") rstrategy = RR_EFFICIENCY;
	else if (rst == "probability") rstrategy = RR_PROBABILITY;
	else if (rst == "none") rstrategy = RR_NONE;
	else {
		std::stringstream ss;
		ss<<"Strategy  '"<<st<<"' for russian roulette path termination unknown. Using \"efficiency\".";
		girlError(GIRL_BADTOKEN,GIRL_WARNING,ss.str().c_str());
		rstrategy = RR_EFFICIENCY;
	}

	return new PathIntegrator(estrategy, rstrategy, maxDepth, RRcontinueProb);
}

static DynamicLoader::RegisterSurfaceIntegrator<PathIntegrator> r("path");
