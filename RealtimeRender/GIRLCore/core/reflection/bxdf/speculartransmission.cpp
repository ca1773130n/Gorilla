/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   speculartransmission.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - speculartransmission.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// speculartransmission.cpp*
#include "speculartransmission.h"
#include "spectrum.h"
#include "spectrumwavelengths.h"

using namespace girl;

bool SpecularTransmission::Sample_f(const TsPack *tspack, const Vector &wo,
	Vector *wi, float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack, bool reverse) const {
	// Figure out which $\eta$ is incident and which is transmitted
	const bool entering = CosTheta(wo) > 0.f;
	float ei = etai, et = etat;

	if(cb != 0.f) {
		// Handle dispersion using cauchy formula
		const float w = tspack->swl->SampleSingle();
		et += (cb * 1000000.f) / (w * w);
	}

	if (!entering && !architectural)
		swap(ei, et);
	// Compute transmitted ray direction
	const float sini2 = SinTheta2(wo);
	const float eta = ei / et;
	const float eta2 = eta * eta;
	const float sint2 = eta2 * sini2;
	// Handle total internal reflection for transmission
	if (sint2 > 1.) {
		*f = 0.f;
		*pdf = 0.f;
		if (pdfBack)
			*pdfBack = 0.f;
		return false;
	}
	float cost = sqrtf(max(0.f, 1.f - sint2));
	if (entering) cost = -cost;
	if (architectural)
		*wi = -wo;
	else
		*wi = Vector(-eta * wo.x, -eta * wo.y, cost);
	*pdf = 1.f;
	if (pdfBack)
		*pdfBack = 1.f;
	SWCSpectrum F;
	if (!architectural) {
		if (reverse) {
			fresnel.Evaluate(tspack, cost, &F);
			*f = (SWCSpectrum(1.f) - F) * T * eta2 / fabsf(cost);
		} else {
			fresnel.Evaluate(tspack, CosTheta(wo), &F);
			*f = (SWCSpectrum(1.f) - F) * T / (eta2 * fabsf(cost));
		}
	} else {
		if (reverse) {
			if (entering) {
				fresnel.Evaluate(tspack, cost, &F);
				*f = (SWCSpectrum(1.f) - F) * T * eta2 / fabsf(CosTheta(wo));
			} else {
				fresnel.Evaluate(tspack, -CosTheta(wo), &F);
				*f = (SWCSpectrum(1.f) - F) * T / (eta2 * fabsf(CosTheta(wo)));
			}
		} else {
			if (entering) {
				fresnel.Evaluate(tspack, CosTheta(wo), &F);
				*f = (SWCSpectrum(1.f) - F) * T / (eta2 * fabsf(CosTheta(wo)));
			} else {
				fresnel.Evaluate(tspack, -cost, &F);
				*f = (SWCSpectrum(1.f) - F) * T * eta2 / fabsf(CosTheta(wo));
			}
		}
	}
	return true;
}
float SpecularTransmission::Weight(const TsPack *tspack, const Vector &wo, bool reverse) const
{
	// Figure out which $\eta$ is incident and which is transmitted
	const bool entering = CosTheta(wo) > 0.f;
	float ei = etai, et = etat;

	if(cb != 0.f) {
		// Handle dispersion using cauchy formula
		const float w = tspack->swl->w[tspack->swl->single_w];
		et += (cb * 1000000.f) / (w * w);
	}

	if (!entering && !architectural)
		swap(ei, et);
	// Compute transmitted ray direction
	const float sini2 = SinTheta2(wo);
	const float eta = ei / et;
	const float eta2 = eta * eta;
	const float sint2 = eta2 * sini2;
	// Handle total internal reflection for transmission
	if (sint2 > 1.f)
		return 0.f;
	float cost = sqrtf(max(0.f, 1.f - sint2));
	if (entering) cost = -cost;
	float factor;
	bool single = tspack->swl->single;
	tspack->swl->single = true;
	SWCSpectrum F;
	if (!architectural) {
		if (reverse) {
			fresnel.Evaluate(tspack, cost, &F);
			factor = eta2 / fabsf(cost);
		} else {
			fresnel.Evaluate(tspack, CosTheta(wo), &F);
			factor = 1.f / (eta2 * fabsf(cost));
		}
	} else {
		if (reverse) {
			if (entering) {
				fresnel.Evaluate(tspack, cost, &F);
				factor = eta2 / fabsf(CosTheta(wo));
			} else {
				fresnel.Evaluate(tspack, -CosTheta(wo), &F);
				factor = 1.f / (eta2 * fabsf(CosTheta(wo)));
			}
		} else {
			if (entering) {
				fresnel.Evaluate(tspack, CosTheta(wo), &F);
				factor = 1.f / (eta2 * fabsf(CosTheta(wo)));
			} else {
				fresnel.Evaluate(tspack, -cost, &F);
				factor = eta2 / fabsf(CosTheta(wo));
			}
		}
	}
	tspack->swl->single = single;
	return (1.f - F.filter(tspack)) * factor;
}
void SpecularTransmission::f(const TsPack *tspack, const Vector &wo, 
							 const Vector &wi, SWCSpectrum *const f) const {
	if (!(architectural && wi == -wo))
		return;
	// Figure out which $\eta$ is incident and which is transmitted
	const bool entering = CosTheta(wo) > 0.f;
	float ei = etai, et = etat;

	if(cb != 0.f) {
		// Handle dispersion using cauchy formula
		const float w = tspack->swl->SampleSingle();
		et += (cb * 1000000.f) / (w * w);
	}

	// Compute transmitted ray direction
	const float sini2 = SinTheta2(wo);
	const float eta = ei / et;
	const float eta2 = eta * eta;
	const float sint2 = eta2 * sini2;
	// Handle total internal reflection for transmission
	if (sint2 > 1.)
		return;	
	float cost = sqrtf(max(0.f, 1.f - sint2));
	if (entering) cost = -cost;
	SWCSpectrum F;
	if (entering) {
		fresnel.Evaluate(tspack, CosTheta(wi), &F);
		f->AddWeighted(1.f / (eta2 * fabsf(CosTheta(wi))), (SWCSpectrum(1.f) - F) * T);
	} else {
		fresnel.Evaluate(tspack, -cost, &F);
		f->AddWeighted(eta2 / fabsf(CosTheta(wi)), (SWCSpectrum(1.f) - F) * T);
	}
}
