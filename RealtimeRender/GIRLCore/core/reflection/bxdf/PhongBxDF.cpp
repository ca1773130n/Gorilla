
// blinnphongbxdf.cpp*
#include "spectrum.h"
#include "mc.h"
#include "PhongBxDF.h"

using namespace girl;

PhongBxDF::PhongBxDF(float nS)
: BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)) 
{
	nShiness = nS;
}

void PhongBxDF::f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const 
{
	float cosTerm = CosTheta(wi);
	cosTerm = powf( cosTerm, nShiness );

	*f *= cosTerm;
}
