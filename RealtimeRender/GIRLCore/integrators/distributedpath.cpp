/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   distributedpath.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - distributedpath.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// distributedpath.cpp*
#include "distributedpath.h"
#include "bxdf.h"
#include "camera.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// DistributedPath Method Definitions
DistributedPath::DistributedPath(LightStrategy st, bool da, int ds, bool dd, bool dg, bool ida, int ids, bool idd, bool idg,
								 int drd, int drs, int dtd, int dts, int grd, int grs, int gtd, int gts, int srd, int std) {
	lightStrategy = st;

	directAll = da;
	directSamples = ds;
	directDiffuse = dd;
	directGlossy = dg;
	indirectAll = ida;
	indirectSamples = ids;
	indirectDiffuse = idd;
	indirectGlossy = idg;
	diffusereflectDepth = drd;
	diffusereflectSamples = drs;
	diffuserefractDepth = dtd;
	diffuserefractSamples = dts;
	glossyreflectDepth = grd;
	glossyreflectSamples = grs;
	glossyrefractDepth = gtd;
	glossyrefractSamples = gts;
	specularreflectDepth = srd;
	specularrefractDepth = std;
}

void DistributedPath::RequestSamples(Sample *sample, const Scene *scene) {
	if (lightStrategy == SAMPLE_AUTOMATIC) {
		if (scene->sampler->IsMutating() || scene->lights.size() > 7)
			lightStrategy = SAMPLE_ONE_UNIFORM;
		else
			lightStrategy = SAMPLE_ALL_UNIFORM;
	}

	// determine maximum depth for samples
	maxDepth = diffusereflectDepth;
	if(diffuserefractDepth > maxDepth) maxDepth = diffuserefractDepth;
	if(glossyreflectDepth > maxDepth) maxDepth = glossyreflectDepth;
	if(glossyrefractDepth > maxDepth) maxDepth = glossyrefractDepth;
	if(specularreflectDepth > maxDepth) maxDepth = specularreflectDepth;
	if(specularrefractDepth > maxDepth) maxDepth = specularrefractDepth;

	// Direct lighting
	// eye vertex
	lightSampleOffset = sample->Add2D(directSamples);
	lightNumOffset = sample->Add1D(directSamples);
	bsdfSampleOffset = sample->Add2D(directSamples);
	bsdfComponentOffset = sample->Add1D(directSamples);
	// remaining vertices
	indirectlightSampleOffset = sample->Add2D(indirectSamples * maxDepth);
	indirectlightNumOffset = sample->Add1D(indirectSamples * maxDepth);
	indirectbsdfSampleOffset = sample->Add2D(indirectSamples * maxDepth);
	indirectbsdfComponentOffset = sample->Add1D(indirectSamples * maxDepth);

	// Diffuse reflection
	// eye vertex
	diffuse_reflectSampleOffset = sample->Add2D(diffusereflectSamples);
	diffuse_reflectComponentOffset = sample->Add1D(diffusereflectSamples);
	// remaining vertices
	indirectdiffuse_reflectSampleOffset = sample->Add2D(diffusereflectDepth);
	indirectdiffuse_reflectComponentOffset = sample->Add1D(diffusereflectDepth);

	// Diffuse refraction
	// eye vertex
	diffuse_refractSampleOffset = sample->Add2D(diffuserefractSamples);
	diffuse_refractComponentOffset = sample->Add1D(diffuserefractSamples);
	// remaining vertices
	indirectdiffuse_refractSampleOffset = sample->Add2D(diffuserefractDepth);
	indirectdiffuse_refractComponentOffset = sample->Add1D(diffuserefractDepth);

	// Glossy reflection
	// eye vertex
	glossy_reflectSampleOffset = sample->Add2D(glossyreflectSamples);
	glossy_reflectComponentOffset = sample->Add1D(glossyreflectSamples);
	// remaining vertices
	indirectglossy_reflectSampleOffset = sample->Add2D(glossyreflectDepth);
	indirectglossy_reflectComponentOffset = sample->Add1D(glossyreflectDepth);

	// Glossy refraction
	// eye vertex
	glossy_refractSampleOffset = sample->Add2D(glossyrefractSamples);
	glossy_refractComponentOffset = sample->Add1D(glossyrefractSamples);
	// remaining vertices
	indirectglossy_refractSampleOffset = sample->Add2D(glossyrefractDepth);
	indirectglossy_refractComponentOffset = sample->Add1D(glossyrefractDepth);

}
void DistributedPath::Preprocess(const TsPack *tspack, const Scene *scene)
{
	// Prepare image buffers
	BufferType type = BUF_TYPE_PER_PIXEL;
	scene->sampler->GetBufferType(&type);
	bufferId = scene->camera->film->RequestBuffer(type, BUF_FRAMEBUFFER, "eye");
}

SWCSpectrum DistributedPath::LiInternal(const TsPack *tspack, const Scene *scene,
		const RayDifferential &ray, const Sample *sample,
		float *alpha, int rayDepth, bool includeEmit, float &nrContribs) const {
	Intersection isect;
	SWCSpectrum L(0.);
	const float time = ray.time; // save time for motion blur
	if (alpha) *alpha = 1.;

	if (scene->Intersect(ray, &isect)) {
		// Dade - collect samples
	//	float *sampleData = sample->sampler->GetLazyValues(const_cast<Sample *>(sample), sampleOffset, rayDepth);
		
		// Evaluate BSDF at hit point
//		BSDF *bsdf = isect.GetBSDF(tspack, ray, fabsf(2.f * bsdfComponent[0] - 1.f));
		BSDF *bsdf = isect.GetBSDF(tspack, ray, fabsf(2.f * tspack->rng->floatValue() - 1.f)); // TODO - REFACT - remove and add random value from sample
		Vector wo = -ray.d;
		const Point &p = bsdf->dgShading.p;
		const Normal &n = bsdf->dgShading.nn;

		// Compute emitted light if ray hit an area light source
		if(includeEmit) {
			const SWCSpectrum Le(isect.Le(tspack, wo));
			if (Le.filter(tspack) > 0.f) {
				L += Le;
				++nrContribs;
			}
		}

		// Compute direct lighting for _DistributedPath_ integrator
		if (scene->lights.size() > 0) {
			int samples = directSamples;
			if(rayDepth > 0) {
				samples = indirectSamples;
			}
			float invsamples = 1.f/samples;
			float *lightSample, *lightNum, *bsdfSample, *bsdfComponent;
			for(int i=0; i<samples; i++) {
				// get samples
				if(rayDepth > 0) {
					lightSample = &sample->twoD[indirectlightSampleOffset][2*i * rayDepth];
					lightNum = &sample->oneD[indirectlightNumOffset][i * rayDepth];
					bsdfSample = &sample->twoD[indirectbsdfSampleOffset][2*i * rayDepth]; 
					bsdfComponent = &sample->oneD[indirectbsdfComponentOffset][i * rayDepth]; 
				} else {
					lightSample = &sample->twoD[lightSampleOffset][2*i];
					lightNum = &sample->oneD[lightNumOffset][i];
					bsdfSample = &sample->twoD[bsdfSampleOffset][2*i]; 
					bsdfComponent = &sample->oneD[bsdfComponentOffset][i]; 
				}

				// Apply direct lighting strategy
				switch (lightStrategy) {
					case SAMPLE_ALL_UNIFORM:
						for (u_int i = 0; i < scene->lights.size(); ++i) {
							const SWCSpectrum Ld(EstimateDirect(tspack, scene, scene->lights[i], p, n, wo, bsdf,
								sample, lightSample[0], lightSample[1], *lightNum, bsdfSample[0], bsdfSample[1], *bsdfComponent));
							if (Ld.filter(tspack) > 0.f) {
								L += invsamples * Ld;
								++nrContribs;
							}
							// TODO add bsdf selection flags
						}
						break;
					case SAMPLE_ONE_UNIFORM:
					{
						const SWCSpectrum Ld(UniformSampleOneLight(tspack, scene, p, n,
							wo, bsdf, sample,
							lightSample, lightNum, bsdfSample, bsdfComponent));
						if (Ld.filter(tspack) > 0.f) {
							L += invsamples * Ld;
							++nrContribs;
						}
						break;
					}
					default:
						break;
				}
			}
		}

		BxDFType flags;
		float pdf;
		Vector wi;
		SWCSpectrum f;
		int samples;
		float invsamples;

		// trace Diffuse reflection & transmission rays
		if (rayDepth < diffusereflectDepth) {
			if(rayDepth > 0) samples = 1;
			else samples = diffusereflectSamples;
			invsamples = 1.f/samples;

			for(int i=0; i<samples; i++) {
				float u1, u2, u3;
				if(rayDepth > 0) {
					u1 = sample->twoD[indirectdiffuse_reflectSampleOffset][2*i * rayDepth];
					u2 = sample->twoD[indirectdiffuse_reflectSampleOffset][(2*i * rayDepth)+1];
					u3 = sample->oneD[indirectdiffuse_reflectComponentOffset][i * rayDepth];
				} else {
					u1 = sample->twoD[diffuse_reflectSampleOffset][2*i];
					u2 = sample->twoD[diffuse_reflectSampleOffset][2*i+1];
					u3 = sample->oneD[diffuse_reflectComponentOffset][i];
				} 

				if (bsdf->Sample_f(tspack, wo, &wi, u1, u2, u3, &f, 
					 &pdf, BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE), &flags)) {
					RayDifferential rd(p, wi);
					rd.time = time;
					L += invsamples * LiInternal(tspack, scene, rd, sample, alpha, rayDepth + 1, false, nrContribs)
						* f * AbsDot(wi, n) / pdf;
				}
			}
		}
		if (rayDepth < diffuserefractDepth) {
			if(rayDepth > 0) samples = 1;
			else samples = diffuserefractSamples;
			invsamples = 1.f/samples;

			for(int i=0; i<samples; i++) {
				float u1, u2, u3;
				if(rayDepth > 0) {
					u1 = sample->twoD[indirectdiffuse_refractSampleOffset][2*i * rayDepth];
					u2 = sample->twoD[indirectdiffuse_refractSampleOffset][(2*i * rayDepth)+1];
					u3 = sample->oneD[indirectdiffuse_refractComponentOffset][i * rayDepth];
				} else {
					u1 = sample->twoD[diffuse_refractSampleOffset][2*i];
					u2 = sample->twoD[diffuse_refractSampleOffset][2*i+1];
					u3 = sample->oneD[diffuse_refractComponentOffset][i];
				} 

				if (bsdf->Sample_f(tspack, wo, &wi, u1, u2, u3, &f, 
					 &pdf, BxDFType(BSDF_TRANSMISSION | BSDF_DIFFUSE), &flags)) {
					RayDifferential rd(p, wi);
					rd.time = time;
					L += invsamples * LiInternal(tspack, scene, rd, sample, alpha, rayDepth + 1, false, nrContribs)
						* f * AbsDot(wi, n) / pdf;
				}
			}
		}

		// trace Glossy reflection & transmission rays
		if (rayDepth < glossyreflectDepth) {
			if(rayDepth > 0) samples = 1;
			else samples = glossyreflectSamples;
			invsamples = 1.f/samples;

			for(int i=0; i<samples; i++) {
				float u1, u2, u3;
				if(rayDepth > 0) {
					u1 = sample->twoD[indirectglossy_reflectSampleOffset][2*i * rayDepth];
					u2 = sample->twoD[indirectglossy_reflectSampleOffset][(2*i * rayDepth)+1];
					u3 = sample->oneD[indirectglossy_reflectComponentOffset][i * rayDepth];
				} else {
					u1 = sample->twoD[glossy_reflectSampleOffset][2*i];
					u2 = sample->twoD[glossy_reflectSampleOffset][2*i+1];
					u3 = sample->oneD[glossy_reflectComponentOffset][i];
				} 

				if (bsdf->Sample_f(tspack, wo, &wi, u1, u2, u3, &f, 
					 &pdf, BxDFType(BSDF_REFLECTION | BSDF_GLOSSY), &flags)) {
					RayDifferential rd(p, wi);
					rd.time = time;
					L += invsamples * LiInternal(tspack, scene, rd, sample, alpha, rayDepth + 1, false, nrContribs)
						* f * AbsDot(wi, n) / pdf;
				}
			}
		}
		if (rayDepth < glossyrefractDepth) {
			if(rayDepth > 0) samples = 1;
			else samples = glossyrefractSamples;
			invsamples = 1.f/samples;

			for(int i=0; i<samples; i++) {
				float u1, u2, u3;
				if(rayDepth > 0) {
					u1 = sample->twoD[indirectglossy_refractSampleOffset][2*i * rayDepth];
					u2 = sample->twoD[indirectglossy_refractSampleOffset][(2*i * rayDepth)+1];
					u3 = sample->oneD[indirectglossy_refractComponentOffset][i * rayDepth];
				} else {
					u1 = sample->twoD[glossy_refractSampleOffset][2*i];
					u2 = sample->twoD[glossy_refractSampleOffset][2*i+1];
					u3 = sample->oneD[glossy_refractComponentOffset][i];
				} 

				if (bsdf->Sample_f(tspack, wo, &wi, u1, u2, u3, &f, 
					&pdf, BxDFType(BSDF_TRANSMISSION | BSDF_GLOSSY), &flags)) {
					RayDifferential rd(p, wi);
					rd.time = time;
					L += invsamples * LiInternal(tspack, scene, rd, sample, alpha, rayDepth + 1, false, nrContribs)
						* f * AbsDot(wi, n) / pdf;
				}
			}
		} 
		
		// trace specular reflection & transmission rays
		if (rayDepth < specularreflectDepth) {
			if (bsdf->Sample_f(tspack, wo, &wi, 1.f, 1.f, 1.f, &f, 
				 &pdf, BxDFType(BSDF_REFLECTION | BSDF_SPECULAR))) {
				RayDifferential rd(p, wi);
				rd.time = time;
				L += LiInternal(tspack, scene, rd, sample, alpha, rayDepth + 1, true, nrContribs) * f * AbsDot(wi, n);
			}
		}
		if (rayDepth < specularrefractDepth) {
			if (bsdf->Sample_f(tspack, wo, &wi, 1.f, 1.f, 1.f, &f, 
				 &pdf, BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR))) {
				RayDifferential rd(p, wi);
				rd.time = time;
				L += LiInternal(tspack, scene, rd, sample, alpha, rayDepth + 1, true, nrContribs) * f * AbsDot(wi, n);
			}
		} 

	} else {
		// Handle ray with no intersection
		for (u_int i = 0; i < scene->lights.size(); ++i) {
			const SWCSpectrum Le(scene->lights[i]->Le(tspack, ray));
			if (Le.filter(tspack) > 0.f) {
				L += scene->lights[i]->Le(tspack, ray);
				++nrContribs;
			}
		}
		if (alpha && L.Black()) *alpha = 0.;
	}

	//return L * scene->volumeIntegrator->Transmittance(tspack, scene, ray, sample, alpha) + scene->volumeIntegrator->Li(tspack, scene, ray, sample, alpha);
	scene->volumeIntegrator->Transmittance(tspack, scene, ray, sample, alpha, &L);
	return L + scene->volumeIntegrator->Li(tspack, scene, ray, sample, alpha);
}

SWCSpectrum DistributedPath::Li(const TsPack *tspack, const Scene *scene,
		const RayDifferential &ray, const Sample *sample,
		float *alpha) const {
	SampleGuard guard(sample->sampler, sample);
	float nrContribs = 0.f;

	sample->AddContribution(sample->imageX, sample->imageY,
		LiInternal(tspack, scene, ray, sample, alpha, 0, true, nrContribs).ToXYZ(tspack),
		alpha ? *alpha : 1.f, bufferId);

	return nrContribs;
}

SurfaceIntegrator* DistributedPath::CreateSurfaceIntegrator(const ParamSet &params) {
	// DirectLight Sampling
	bool directall = params.FindOneBool("directsampleall", true);
	int directsamples = params.FindOneInt("directsamples", 1);
	bool directdiffuse = params.FindOneBool("directdiffuse", true);
	bool directglossy = params.FindOneBool("directglossy", true);
	// Indirect DirectLight Sampling
	bool indirectall = params.FindOneBool("indirectsampleall", false);
	int indirectsamples = params.FindOneInt("indirectsamples", 1);
	bool indirectdiffuse = params.FindOneBool("indirectdiffuse", true);
	bool indirectglossy = params.FindOneBool("indirectglossy", true);

	// Diffuse
	int diffusereflectdepth = params.FindOneInt("diffusereflectdepth", 3);
	int diffusereflectsamples = params.FindOneInt("diffusereflectsamples", 1);
	int diffuserefractdepth = params.FindOneInt("diffuserefractdepth", 5);
	int diffuserefractsamples = params.FindOneInt("diffuserefractsamples", 1);
	// Glossy
	int glossyreflectdepth = params.FindOneInt("glossyreflectdepth", 2);
	int glossyreflectsamples = params.FindOneInt("glossyreflectsamples", 1);
	int glossyrefractdepth = params.FindOneInt("glossyrefractdepth", 5);
	int glossyrefractsamples = params.FindOneInt("glossyrefractsamples", 1);
	// Specular
	int specularreflectdepth = params.FindOneInt("specularreflectdepth", 2);
	int specularrefractdepth = params.FindOneInt("specularrefractdepth", 5);

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

	return new DistributedPath(estrategy, directall, directsamples,
		directdiffuse, directglossy, indirectall, indirectsamples, indirectdiffuse, indirectglossy,
		diffusereflectdepth, diffusereflectsamples, diffuserefractdepth, diffuserefractsamples, glossyreflectdepth, glossyreflectsamples, 
		glossyrefractdepth, glossyrefractsamples, specularreflectdepth, specularrefractdepth);
}

static DynamicLoader::RegisterSurfaceIntegrator<DistributedPath> r("distributedpath");
