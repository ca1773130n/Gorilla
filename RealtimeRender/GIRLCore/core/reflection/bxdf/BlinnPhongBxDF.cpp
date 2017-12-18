
// blinnphongbxdf.cpp*
#include "spectrum.h"
#include "mc.h"
#include "microfacetdistribution.h"
#include "BlinnPhongBxDF.h"

using namespace girl;

BlinnPhongBxDF::BlinnPhongBxDF(const SWCSpectrum &kd, const SWCSpectrum &ks, MicrofacetDistribution *dist)
: BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)) 
{
	Kd = kd;
	Ks = ks;
	distribution = dist;
}

//  [11/21/2008 Sung-Soo Kim]
BlinnPhongBxDF::BlinnPhongBxDF(const SWCSpectrum &kd, const SWCSpectrum &ks, MicrofacetDistribution *dist, BxDFType type) 
: BxDF(type) 
{
	Kd = kd;
	Ks = ks;
	distribution = dist;
}

void BlinnPhongBxDF::f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const 
{
  *f += Kd * INV_PI;

  Vector wh = Normalize(wi + wo);

  *f += Ks * distribution->D(wh);
}
