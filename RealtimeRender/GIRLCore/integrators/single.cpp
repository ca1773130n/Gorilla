/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   single.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - single.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// single.cpp*
#include "single.h"
#include "light.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// SingleScattering Method Definitions
void SingleScattering::RequestSamples(Sample *sample,
		const Scene *scene) {
	tauSampleOffset = sample->Add1D(1);
	scatterSampleOffset = sample->Add1D(1);
}

void SingleScattering::Transmittance(const TsPack *tspack, const Scene *scene,
		const Ray &ray, const Sample *sample, float *alpha, SWCSpectrum *const L) const {
	if (!scene->volumeRegion) 
		return;
	//float step = sample ? stepSize : 4.f * stepSize;
	float step = stepSize; // TODO - handle varying step size
	float offset = sample->oneD[tauSampleOffset][0];
	SWCSpectrum tau = SWCSpectrum(tspack, scene->volumeRegion->Tau(ray, step, offset));
	*L *= Exp(-tau);
}

SWCSpectrum SingleScattering::Li(const TsPack *tspack, const Scene *scene,
		const RayDifferential &ray, const Sample *sample,
		float *alpha) const {
	VolumeRegion *vr = scene->volumeRegion;
	float t0, t1;
	if (!vr || !vr->IntersectP(ray, &t0, &t1)) return 0.f;
	// Do single scattering volume integration in _vr_
	SWCSpectrum Lv(0.);
	// Prepare for volume integration stepping
	int N = Ceil2Int((t1-t0) / stepSize);
	float step = (t1 - t0) / N;
	SWCSpectrum Tr(1.f);
	Point p = ray(t0), pPrev;
	Vector w = -ray.d;
	t0 += sample->oneD[scatterSampleOffset][0] * step;
	// Compute sample patterns for single scattering samples
	float *samp = (float *)alloca(4 * N * sizeof(float));
	LatinHypercube(tspack, samp, N, 4);
	int sampOffset = 0;
	for (int i = 0; i < N; ++i, t0 += step) {
		// Advance to sample at _t0_ and update _T_
		pPrev = p;
		p = ray(t0);

		SWCSpectrum stepTau = SWCSpectrum(tspack, vr->Tau(Ray(pPrev, p - pPrev, 0, 1),
			.5f * stepSize, tspack->rng->floatValue())); // TODO - REFACT - remove and add random value from sample
		Tr *= Exp(-stepTau);
		// Possibly terminate raymarching if transmittance is small
		if (Tr.filter(tspack) < 1e-3) {
			const float continueProb = .5f;
			if (tspack->rng->floatValue() > continueProb) break; // TODO - REFACT - remove and add random value from sample
			Tr /= continueProb;
		}

		// Compute single-scattering source term at _p_
		Lv += Tr * SWCSpectrum(tspack, vr->Lve(p, w));

		SWCSpectrum ss = SWCSpectrum(tspack, vr->sigma_s(p, w));
		if (!ss.Black() && scene->lights.size() > 0) {
			int nLights = scene->lights.size();
			int lightNum =
				min(Floor2Int(samp[sampOffset] * nLights),
					nLights-1);
			Light *light = scene->lights[lightNum];

			// Add contribution of _light_ due to scattering at _p_
			float pdf;
			VisibilityTester vis;
			Vector wo;
			float u1 = samp[sampOffset+1], u2 = samp[sampOffset+2], u3 = samp[sampOffset+3];
			SWCSpectrum L = light->Sample_L(tspack, p, u1, u2, u3, &wo, &pdf, &vis);

			// Dade - use the new TestOcclusion() method
			SWCSpectrum occlusion;
			if ((!L.Black()) && (pdf > 0.0f) && vis.TestOcclusion(tspack, scene, &occlusion)) {	
				SWCSpectrum Ld = L * occlusion;
				vis.Transmittance(tspack, scene, sample, &Ld);
				Lv += Tr * ss * vr->p(p, w, -wo) *
					  Ld * float(nLights) / pdf;
			}
		}
		sampOffset += 4;
	}
	return Lv * step;
}

VolumeIntegrator* SingleScattering::CreateVolumeIntegrator(const ParamSet &params) {
	float stepSize  = params.FindOneFloat("stepsize", 1.f);
	return new SingleScattering(stepSize);
}

static DynamicLoader::RegisterVolumeIntegrator<SingleScattering> r("single");
