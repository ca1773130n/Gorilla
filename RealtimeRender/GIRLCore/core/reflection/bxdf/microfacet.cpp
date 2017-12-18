/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   microfacet.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - microfacet.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// microfacet.cpp*
#include "microfacet.h"
#include "spectrum.h"
#include "mc.h"
#include "fresnel.h"
#include "microfacetdistribution.h"

using namespace girl;

Microfacet::Microfacet(const SWCSpectrum &reflectance,
                       Fresnel *f,
					   MicrofacetDistribution *d)
	: BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
	 R(reflectance), distribution(d), fresnel(f) {
}

void Microfacet::f(const TsPack *tspack, const Vector &wo, 
				   const Vector &wi, SWCSpectrum *const f) const {
	float cosThetaO = fabsf(CosTheta(wo));
	float cosThetaI = fabsf(CosTheta(wi));
	Vector wh = Normalize(wi + wo);
	float cosThetaH = Dot(wi, wh);
	SWCSpectrum F;
	fresnel->Evaluate(tspack, cosThetaH, &F);
	f->AddWeighted(distribution->D(wh) * G(wo, wi, wh) /
		(4.f * cosThetaI * cosThetaO), R * F);
}

bool Microfacet::Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi,
						  float u1, float u2, SWCSpectrum *const f, float *pdf, 
						  float *pdfBack,	bool reverse) const {
	distribution->Sample_f(wo, wi, u1, u2, pdf);
	if (pdfBack)
		*pdfBack = Pdf(tspack, *wi, wo);
	if (!SameHemisphere(wo, *wi)) 
		return false;

	*f = SWCSpectrum(0.f);
	if (reverse) {
		this->f(tspack, *wi, wo, f);
//		*f *= (wo.z / wi->z);	
	}
	else
		this->f(tspack, wo, *wi, f);
	return true;
}
float Microfacet::Pdf(const TsPack *tspack, const Vector &wo,
		const Vector &wi) const {
	if (!SameHemisphere(wo, wi)) return 0.f;
	return distribution->Pdf(wo, wi);
}

