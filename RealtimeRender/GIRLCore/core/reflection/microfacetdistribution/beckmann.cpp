/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   beckmann.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - beckmann.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// beckmann.cpp*
#include "beckmann.h"
#include "bxdf.h"

using namespace girl;

Beckmann::Beckmann(float rms) {
  r = rms;
}

float Beckmann::D(const Vector &wh) const {
  float costhetah = CosTheta(wh);
  float theta = acos(costhetah);
  float tanthetah = tan(theta);

  float dfac = tanthetah / r;

  return exp(-(dfac * dfac)) / (r * r * powf(costhetah, 4.0));
}

void Beckmann::Sample_f(const Vector &wo, Vector *wi, float u1, float u2, float *pdf) const {
  // Compute sampled half-angle vector $\wh$ for Beckmann distribution
  // Adapted from B. Walter et al, Microfacet Models for Refraction, Eurographics Symposium on Rendering, 2007, page 7

  float theta = atan (sqrt (-(r * r) * log(1.0 - u1)));
  float costheta = cos (theta);
  float sintheta = sqrtf(max(0.f, 1.f - costheta*costheta));
  float phi = u2 * 2.f * M_PI;

  Vector H = SphericalDirection(sintheta, costheta, phi);

  if (!SameHemisphere(wo, H))
    H.z *= -1.f;

  // Compute incident direction by reflecting about $\wh$
  *wi = -wo + 2.f * Dot(wo, H) * H;

  // Compute PDF for \wi from Beckmann distribution - note that the inverse of the integral over
  // the Beckmann distribution is not available in closed form, so this is not really correct
  // (see Kelemen and Szirmay-Kalos / Microfacet Based BRDF Model, Eurographics 2001)

  float conversion_factor = 1.0 / (4.f * Dot(wo, H));
  float beckmann_pdf = conversion_factor * D(H);

  *pdf = beckmann_pdf;
}

// NB: See note above!
float Beckmann::Pdf(const Vector &wo, const Vector &wi) const {
  Vector H = Normalize(wo + wi);
  float conversion_factor = 1.0 / 4.f * Dot(wo, H);
  float beckmann_pdf = conversion_factor * D(H);

  return beckmann_pdf;
}

