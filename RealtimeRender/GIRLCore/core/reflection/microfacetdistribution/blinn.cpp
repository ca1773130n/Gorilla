/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   blinn.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - blinn.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// blinn.cpp*
#include "blinn.h"

using namespace girl;

void Blinn::Sample_f(const Vector &wo, Vector *wi,
		float u1, float u2, float *pdf) const {
	// Compute sampled half-angle vector $\wh$ for Blinn distribution
	float costheta = powf(u1, 1.f / (exponent+1.0f));
	float sintheta = sqrtf(max(0.f, 1.f - costheta*costheta));
	float phi = u2 * 2.f * M_PI;
	Vector H = SphericalDirection(sintheta, costheta, phi);
	if (!SameHemisphere(wo, H)) H.z *= -1.f;
	// Compute incident direction by reflecting about $\wh$
	*wi = -wo + 2.f * Dot(wo, H) * H;
	// Compute PDF for \wi from Blinn distribution
	float blinn_pdf = ((exponent + 1.f) *
	                   powf(costheta, exponent)) /
		(2.f * M_PI * 4.f * Dot(wo, H));

	// NOTE - Ratow - Make sure pdf is non-negative
	if (blinn_pdf < 0.f)
		*pdf = 0.f;
	else
		*pdf = blinn_pdf;
}
float Blinn::Pdf(const Vector &wo, const Vector &wi) const {
	Vector H = Normalize(wo + wi);
	float costheta = fabsf(H.z);
	// Compute PDF for \wi from Blinn distribution
	float blinn_pdf = ((exponent + 1.f) *
	                   powf(costheta, exponent)) /
		(2.f * M_PI * 4.f * Dot(wo, H));
	return blinn_pdf;
}
