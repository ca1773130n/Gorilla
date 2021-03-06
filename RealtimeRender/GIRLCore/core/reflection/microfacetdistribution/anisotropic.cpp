/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   anisotropic.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   Anisotropic distribution model (Ashikhmin-Shirley model)
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-12-14  :   First version - anisotropic.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// anisotropic.cpp*
#include "anisotropic.h"

using namespace girl;


void Anisotropic::Sample_f(const Vector &wo, Vector *wi,
		float u1, float u2, float *pdf) const {
	// Sample from first quadrant and remap to hemisphere to sample \wh
	float phi, costheta;
	if (u1 < .25f) {
		sampleFirstQuadrant(4.f * u1, u2, &phi, &costheta);
	} else if (u1 < .5f) {
		u1 = 4.f * (.5f - u1);
		sampleFirstQuadrant(u1, u2, &phi, &costheta);
		phi = M_PI - phi;
	} else if (u1 < .75f) {
		u1 = 4.f * (u1 - .5f);
		sampleFirstQuadrant(u1, u2, &phi, &costheta);
		phi += M_PI;
	} else {
		u1 = 4.f * (1.f - u1);
		sampleFirstQuadrant(u1, u2, &phi, &costheta);
		phi = 2.f * M_PI - phi;
	}
	float sintheta = sqrtf(max(0.f, 1.f - costheta*costheta));
	Vector H = SphericalDirection(sintheta, costheta, phi);
	if (Dot(wo, H) < 0.f) H = -H;
	// Compute incident direction by reflecting about $\wh$
	*wi = -wo + 2.f * Dot(wo, H) * H;
	// Compute PDF for \wi from Anisotropic distribution
	float anisotropic_pdf = D(H) / (4.f * Dot(wo, H));
	*pdf = anisotropic_pdf;
}
void Anisotropic::sampleFirstQuadrant(float u1, float u2,
		float *phi, float *costheta) const {
	if (ex == ey)
		*phi = M_PI * u1 * 0.5f;
	else
		*phi = atanf(sqrtf((ex+1)/(ey+1)) *
			tanf(M_PI * u1 * 0.5f));
	float cosphi = cosf(*phi), sinphi = sinf(*phi);
	*costheta = powf(u2, 1.f/(ex * cosphi * cosphi +
		ey * sinphi * sinphi + 1));
}
float Anisotropic::Pdf(const Vector &wo,
		const Vector &wi) const {
	Vector H = Normalize(wo + wi);
	// Compute PDF for \wi from Anisotropic distribution
	float anisotropic_pdf = D(H) / (4.f * Dot(wo, H));
	return anisotropic_pdf;
}

