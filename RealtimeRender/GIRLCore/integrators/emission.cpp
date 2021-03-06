/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   emission.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - emission.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// emission.cpp*
#include "emission.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// EmissionIntegrator Method Definitions
void EmissionIntegrator::RequestSamples(Sample *sample,
		const Scene *scene) {
	tauSampleOffset = sample->Add1D(1);
	scatterSampleOffset = sample->Add1D(1);
}

void EmissionIntegrator::Transmittance(const TsPack *tspack, const Scene *scene,
		const Ray &ray, const Sample *sample, float *alpha, SWCSpectrum *const L) const {
	if (!scene->volumeRegion) 
		return;
	//float step = sample ? stepSize : 4.f * stepSize;
	float step = stepSize;
	float offset = sample->oneD[tauSampleOffset][0];
	SWCSpectrum tau =
		SWCSpectrum(tspack, scene->volumeRegion->Tau(ray, step, offset));
	*L *= Exp(-tau);
}
SWCSpectrum EmissionIntegrator::Li(const TsPack *tspack, const Scene *scene,
		const RayDifferential &ray, const Sample *sample,
		float *alpha) const {
	VolumeRegion *vr = scene->volumeRegion;
	float t0, t1;
	if (!vr || !vr->IntersectP(ray, &t0, &t1)) return 0.f;
	// Do emission-only volume integration in _vr_
	SWCSpectrum Lv(0.);
	// Prepare for volume integration stepping
	int N = Ceil2Int((t1-t0) / stepSize);
	float step = (t1 - t0) / N;
	SWCSpectrum Tr(1.f);
	Point p = ray(t0), pPrev;
	Vector w = -ray.d;
	t0 += sample->oneD[scatterSampleOffset][0] * step;
	for (int i = 0; i < N; ++i, t0 += step) {
		// Advance to sample at _t0_ and update _T_
		pPrev = p;
		p = ray(t0);
		SWCSpectrum stepTau = SWCSpectrum(tspack, vr->Tau(Ray(pPrev, p - pPrev, 0, 1),
			.5f * stepSize, tspack->rng->floatValue()));	// TODO - REFACT - remove and add random value from sample
		Tr *= Exp(-stepTau);
		// Possibly terminate raymarching if transmittance is small
		if (Tr.filter(tspack) < 1e-3) {
			const float continueProb = .5f;
			if (tspack->rng->floatValue() > continueProb) break; // TODO - REFACT - remove and add random value from sample
			Tr /= continueProb;
		}
		// Compute emission-only source term at _p_
		Lv += Tr * SWCSpectrum(tspack, vr->Lve(p, w));
	}
	return Lv * step;
}
VolumeIntegrator* EmissionIntegrator::CreateVolumeIntegrator(const ParamSet &params) {
	float stepSize  = params.FindOneFloat("stepsize", 1.f);
	return new EmissionIntegrator(stepSize);
}

static DynamicLoader::RegisterVolumeIntegrator<EmissionIntegrator> r("emission");
