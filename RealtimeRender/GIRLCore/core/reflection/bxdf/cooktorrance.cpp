/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   cooktorrance.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - cooktorrance.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// cooktorrance.cpp*
#include "cooktorrance.h"
#include "spectrum.h"
#include "mc.h"
#include "microfacetdistribution.h"
#include "fresnel.h"

using namespace girl;

CookTorrance::CookTorrance(const SWCSpectrum &kd, u_int nl,
						   const SWCSpectrum *ks,
						   MicrofacetDistribution **dist, Fresnel **fres) : BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)) {
  KD = kd;
  KS = ks;
  nLobes = nl;
  distribution = dist;
  fresnel = fres;
}

//  [11/21/2008 Sung-Soo Kim]
CookTorrance::CookTorrance(const SWCSpectrum &kd, u_int nl,
						   const SWCSpectrum *ks,
						   MicrofacetDistribution **dist, Fresnel **fres, 
						   BxDFType type) : BxDF(type) {
   KD = kd;
   KS = ks;
   nLobes = nl;
   distribution = dist;
   fresnel = fres;
}

void CookTorrance::f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const {
  *f += KD * INV_PI;

  float cosThetaO = fabsf(CosTheta(wo));
  float cosThetaI = fabsf(CosTheta(wi));
  Vector wh = Normalize(wi + wo);
  float cosThetaH = Dot(wi, wh);
  float cG = G(wo, wi, wh);

  for (u_int i = 0; i < nLobes; i++) {
	// Add contribution for $i$th Cook-Torrance lobe
	  SWCSpectrum F;
	  fresnel[i]->Evaluate(tspack, cosThetaH, &F);
	*f += KS[i] * distribution[i]->D(wh) * cG * F / (M_PI * cosThetaI * cosThetaO);
  }
}

float CookTorrance::G(const Vector &wo, const Vector &wi, const Vector &wh) const {
  float NdotWh = fabsf(CosTheta(wh));
  float NdotWo = fabsf(CosTheta(wo));
  float NdotWi = fabsf(CosTheta(wi));
  float WOdotWh = AbsDot(wo, wh);
  return min(1.f, min((2.f * NdotWh * NdotWo / WOdotWh), (2.f * NdotWh * NdotWi / WOdotWh)));
}

bool CookTorrance::Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi, float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack, bool reverse) const {
	// Pick a random component
	u_int comp = tspack->rng->uintValue() % (nLobes+1);								// TODO - REFACT - remove and add random value from sample

	if (comp == nLobes) {
	// The diffuse term; cosine-sample the hemisphere, flipping the direction if necessary
	*wi = CosineSampleHemisphere(u1, u2);
	if (wo.z < 0.)
		wi->z *= -1.f;
	}
	else {
	// Sample lobe number _comp_ for Cook-Torrance BRDF
	distribution[comp]->Sample_f(wo, wi, u1, u2, pdf);
	}
	*pdf = Pdf(tspack, wo, *wi);
	if (*pdf == 0.f) {
		if (pdfBack)
			*pdfBack = 0.f;
		return false;
	}
	if (pdfBack)
		*pdfBack = Pdf(tspack, *wi, wo);

	*f = SWCSpectrum(0.0);
	if (reverse) {
		this->f(tspack, *wi, wo, f);
//		*f *= (wo.z / wi->z);
	}
	else
		this->f(tspack, wo, *wi, f);
	return true;
}

float CookTorrance::Pdf(const TsPack *tspack, const Vector &wo, const Vector &wi) const {
  if (!SameHemisphere(wo, wi))
	return 0.f;

  // Average of all pdf's
  float pdfSum = fabsf(wi.z) * INV_PI;

  for (u_int i = 0; i < nLobes; ++i) {
	pdfSum += distribution[i]->Pdf(wo, wi);
  }
  return pdfSum / (1.f + nLobes);
}

