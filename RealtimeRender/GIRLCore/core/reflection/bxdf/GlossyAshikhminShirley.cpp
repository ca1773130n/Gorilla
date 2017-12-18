/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   GlossyAshikhminShirley.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - GlossyAshikhminShirley.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// fresnelblend.cpp*
#include "GlossyAshikhminShirley.h"
#include "spectrum.h"
#include "mc.h"
#include "microfacetdistribution.h"
#include "fresnelslick.h"

using namespace girl;

GlossyAshikhminShirley::GlossyAshikhminShirley(const SWCSpectrum &d,
                           const SWCSpectrum &s,
						   float f0,
						   MicrofacetDistribution *dist)
	: BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
	  Rd(d), Rs(s), F0(f0) {
	distribution = dist;
}
void GlossyAshikhminShirley::f(const TsPack *tspack, const Vector &wo, 
					 const Vector &wi, SWCSpectrum *const f) const 
{
	Vector wh = Normalize(wi + wo);
	float cosThetaH = Dot(wi, wh);

	Fresnel **F = (Fresnel **)BSDF::Alloc(tspack, 1 * sizeof(Fresnel *));
	F[0] = BSDF_ALLOC(tspack, FresnelSlick)(F0);

	SWCSpectrum fSpectrum;
	F[0]->Evaluate(tspack, cosThetaH, &fSpectrum);

	Vector H = Normalize(wi + wo);

	// specular part
	SWCSpectrum specular = distribution->D(H) /
		(8.f * M_PI* AbsDot(wi, H) * 
		max(fabsf(CosTheta(wi)), fabsf(CosTheta(wo)))) * 
		fSpectrum;

	*f += Rd * INV_PI + specular;
}

bool GlossyAshikhminShirley::Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi,
							float u1, float u2, SWCSpectrum *const f, float *pdf, 
							float *pdfBack, bool reverse) const {
	u1 *= 2.f;
	if (u1 < 1.f) {
		// Cosine-sample the hemisphere, flipping the direction if necessary
		*wi = CosineSampleHemisphere(u1, u2);
		if (wo.z < 0.) wi->z *= -1.f;
	}
	else {
		u1 -= 1.f;
		distribution->Sample_f(wo, wi, u1, u2, pdf);
	}
	*pdf = Pdf(tspack, wo, *wi);
	if (*pdf == 0.f) {
		if (pdfBack)
			*pdfBack = 0.f;
		return false;
	}
	if (pdfBack)
		*pdfBack = Pdf(tspack, *wi, wo);

	*f = SWCSpectrum(0.f);
	if (reverse) {
		this->f(tspack, *wi, wo, f);
	}
	else
		this->f(tspack, wo, *wi, f);
	return true;
}
float GlossyAshikhminShirley::Pdf(const TsPack *tspack, const Vector &wo,
		const Vector &wi) const {
	if (!SameHemisphere(wo, wi)) return 0.f;
	return .5f * (fabsf(wi.z) * INV_PI +
		distribution->Pdf(wo, wi));
}

