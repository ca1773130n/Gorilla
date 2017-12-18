/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   wardisotropic.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - wardisotropic.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// wardisotropic.cpp*
#include "wardisotropic.h"
#include "bxdf.h"

using namespace girl;

// Ward isotropic distribution, adapted from Kelemen and Szirmay-Kalos / Microfacet Based BRDF Model, Eurographics 2001
WardIsotropic::WardIsotropic(float rms) {
  r = rms;
}

float WardIsotropic::D(const Vector &wh) const {
  float costhetah = CosTheta(wh);
  float theta = acos(costhetah);
  float tanthetah = tan(theta);

  float dfac = tanthetah / r;

  return exp(-(dfac * dfac)) / (M_PI * r * r * powf(costhetah, 3.0));
}

void WardIsotropic::Sample_f(const Vector &wo, Vector *wi, float u1, float u2, float *pdf) const {
  // Compute sampled half-angle vector $\wh$ for Ward distribution

  float theta = atan (r * sqrt (-log(1.0 - u1)));
  float costheta = cos (theta);
  float sintheta = sqrtf(max(0.f, 1.f - costheta*costheta));
  float phi = u2 * 2.f * M_PI;

  Vector H = SphericalDirection(sintheta, costheta, phi);

  if (!SameHemisphere(wo, H))
    H.z *= -1.f;

  // Compute incident direction by reflecting about $\wh$
  *wi = -wo + 2.f * Dot(wo, H) * H;

  // Compute PDF for \wi from isotropic Ward distribution

  float conversion_factor = 1.0 / (4.f * Dot(wo, H));
  float ward_pdf = conversion_factor * D(H);

  *pdf = ward_pdf;
}

float WardIsotropic::Pdf(const Vector &wo, const Vector &wi) const {
  Vector H = Normalize(wo + wi);
  float conversion_factor = 1.0 / 4.f * Dot(wo, H);
  float ward_pdf = conversion_factor * D(H);

  return ward_pdf;
}

