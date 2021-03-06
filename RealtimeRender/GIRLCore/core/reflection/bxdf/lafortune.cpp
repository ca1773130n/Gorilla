/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   lafortune.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - lafortune.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// lafortune.cpp*
#include "lafortune.h"
#include "spectrum.h"
#include "mc.h"

using namespace girl;

Lafortune::Lafortune(const SWCSpectrum &r, u_int nl,
	const SWCSpectrum *xx,
	const SWCSpectrum *yy,
	const SWCSpectrum *zz,
	const SWCSpectrum *e, BxDFType t)
	: BxDF(t), R(r) {
	nLobes = nl;
	x = xx;
	y = yy;
	z = zz;
	exponent = e;
}
void Lafortune::f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const {
	*f += R * INV_PI;
	for (u_int i = 0; i < nLobes; ++i) {
		// Add contribution for $i$th Phong lobe
		SWCSpectrum v = x[i] * wo.x * wi.x + y[i] * wo.y * wi.y +
			z[i] * wo.z * wi.z;
		*f += v.Pow(exponent[i]);
	}
}

bool Lafortune::Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi,
		float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack, bool reverse) const {
			*pdf = 0.f;
	u_int comp = tspack->rng->uintValue() % (nLobes+1);										/// TODO - REFACT - remove and add random value from sample
	if (comp == nLobes) {
		// Cosine-sample the hemisphere, flipping the direction if necessary
		*wi = CosineSampleHemisphere(u1, u2);
		if (wo.z < 0.) wi->z *= -1.f;
	}
	else {
		// Sample lobe _comp_ for Lafortune BRDF
		float xlum = x[comp].y(tspack);
		float ylum = y[comp].y(tspack);
		float zlum = z[comp].y(tspack);
		float costheta = powf(u1, 1.f / (.8f * exponent[comp].y(tspack) + 1));
		float sintheta = sqrtf(max(0.f, 1.f - costheta*costheta));
		float phi = u2 * 2.f * M_PI;
		Vector lobeCenter = Normalize(Vector(xlum * wo.x, ylum * wo.y, zlum * wo.z));
		Vector lobeX, lobeY;
		CoordinateSystem(lobeCenter, &lobeX, &lobeY);
		*wi = SphericalDirection(sintheta, costheta, phi, lobeX, lobeY,
			lobeCenter);
	}
	*pdf = Pdf(tspack, wo, *wi);
	if (pdfBack)
		*pdfBack = Pdf(tspack, *wi, wo);
	if (!SameHemisphere(wo, *wi)) 
		return false;
	*f = SWCSpectrum(0.0);
	if (reverse) {
		this->f(tspack, *wi, wo, f);
//		*f *= (wo.z / wi->z);
	}
	else
		this->f(tspack, wo, *wi, f);
	return true;
}


float Lafortune::Pdf(const TsPack *tspack, const Vector &wo, const Vector &wi) const {
	if (!SameHemisphere(wo, wi)) return 0.f;
	float pdfSum = fabsf(wi.z) * INV_PI;
	for (u_int i = 0; i < nLobes; ++i) {
		float xlum = x[i].y(tspack);
		float ylum = y[i].y(tspack);
		float zlum = z[i].y(tspack);
		Vector lobeCenter =
			Normalize(Vector(wo.x * xlum, wo.y * ylum, wo.z * zlum));
		float e = .8f * exponent[i].y(tspack);
		pdfSum += (e + 1.f) * powf(max(0.f, Dot(wi, lobeCenter)), e);
	}
	return pdfSum / (1.f + nLobes);
}

