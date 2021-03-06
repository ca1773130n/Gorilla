/***************************************************************************
 *   Copyright (C) 1998-2008 by authors (see AUTHORS.txt )                 *
 *                                                                         *
 *   This file is part of GirlRender.                                       *
 *                                                                         *
 *   Girl Renderer is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Girl Renderer is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *   This project is based on PBRT ; see http://www.pbrt.org               *
 *   Girl Renderer website : http://www.girlrender.net                       *
 ***************************************************************************/

// bxdf.cpp*
#include "bxdf.h"
#include "spectrum.h"
#include "spectrumwavelengths.h"
#include "mc.h"
#include "sampling.h"

using namespace girl;

// BxDF Method Definitions
void BRDFToBTDF::f(const TsPack *tspack, const Vector &wo,
	const Vector &wi, SWCSpectrum *const f) const
{
	if (etai == etat) {
		brdf->f(tspack, wo, otherHemisphere(wi), f);
		return;
	}
	// Figure out which $\eta$ is incident and which is transmitted
	const bool entering = CosTheta(wo) > 0.f;
	float ei = etai, et = etat;

	if(cb != 0.f) {
		// Handle dispersion using cauchy formula
		const float w = tspack->swl->SampleSingle();
		et += (cb * 1000000.f) / (w * w);
	}

	if (!entering)
		swap(ei, et);
	// Compute transmitted ray direction
	const float eta = ei / et;
	Vector H(Normalize(eta * wo + wi));
	const float cos1 = Dot(wo, H);
	if ((entering && cos1 < 0.f) || (!entering && cos1 > 0.f))
		H = -H;
	if (H.z < 0.f)
		return;
	Vector wiR(2.f * cos1 * H - wo);
	SWCSpectrum tf(0.f);
	brdf->f(tspack, wo, wiR, &tf);
	tf *= fabsf(wiR.z / (wi.z * eta * eta));
	*f += tf;
}
bool BRDFToBTDF::Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi,
	float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack,
	bool reverse) const
{
	if (etai == etat) {
		if (!brdf->Sample_f(tspack, wo, wi, u1, u2, f, pdf, pdfBack, reverse))
			return false;
		*wi = otherHemisphere(*wi);
		return true;
	}
	if (!brdf->Sample_f(tspack, wo, wi, u1, u2, f, pdf, pdfBack, reverse))
		return false;
	Vector H(Normalize(wo + *wi));
	if (H.z < 0.f)
		H = -H;
	const float cosi = Dot(wo, H);
	// Figure out which $\eta$ is incident and which is transmitted
	const bool entering = cosi > 0.f;
	float ei = etai, et = etat;

	if(cb != 0.f) {
		// Handle dispersion using cauchy formula
		const float w = tspack->swl->SampleSingle();
		et += (cb * 1000000.f) / (w * w);
	}

	if (!entering)
		swap(ei, et);
	// Compute transmitted ray direction
	const float sini2 = max(0.f, 1.f - cosi * cosi);
	const float eta = ei / et;
	const float eta2 = eta * eta;
	const float sint2 = eta2 * sini2;
	// Handle total internal reflection for transmission
	if (sint2 > 1.) {
		*pdf = 0.f;
		if (pdfBack)
			*pdfBack = 0.f;
		return false;
	}
	float cost = sqrtf(max(0.f, 1.f - sint2));
	if (entering)
		cost = -cost;
	*wi = (cost + eta * cosi) * H - eta * wo;
	if (reverse)
		*f *= eta2;
	else
		*f /= eta2;
	return true;
}

bool BxDF::Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi,
	float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack,
	bool reverse) const
{
	// Cosine-sample the hemisphere, flipping the direction if necessary
	*wi = CosineSampleHemisphere(u1, u2);
	if (wo.z < 0.) wi->z *= -1.f;
	*pdf = Pdf(tspack, wo, *wi);
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
float BxDF::Pdf(const TsPack *tspack, const Vector &wo, const Vector &wi) const {
	return
		SameHemisphere(wo, wi) ? fabsf(wi.z) * INV_PI : 0.f;
}
float BRDFToBTDF::Pdf(const TsPack *tspack, const Vector &wo,
		const Vector &wi) const
{
	if (etai == etat)
		return brdf->Pdf(tspack, wo, otherHemisphere(wi));
	// Figure out which $\eta$ is incident and which is transmitted
	const bool entering = CosTheta(wo) > 0.f;
	float ei = etai, et = etat;

	if(cb != 0.f) {
		// Handle dispersion using cauchy formula
		const float w = tspack->swl->SampleSingle();
		et += (cb * 1000000.f) / (w * w);
	}

	if (!entering)
		swap(ei, et);
	// Compute transmitted ray direction
	const float eta = ei / et;
	Vector H(Normalize(eta * wo + wi));
	const float cos1 = Dot(wo, H);
	if ((entering && cos1 < 0.f) || (!entering && cos1 > 0.f))
		H = -H;
	if (H.z < 0.f)
		return 0.f;
	return brdf->Pdf(tspack, wo, 2.f * Dot(wo, H) * H - wo);
}

SWCSpectrum BxDF::rho(const TsPack *tspack, const Vector &w, int nSamples,
		float *samples) const {
	if (!samples) {
		samples =
			(float *)alloca(2 * nSamples * sizeof(float));
		LatinHypercube(tspack, samples, nSamples, 2);
	}
	SWCSpectrum r = 0.;
	for (int i = 0; i < nSamples; ++i) {
		// Estimate one term of $\rho_{dh}$
		Vector wi;
		float pdf = 0.f;
		SWCSpectrum f(0.f);		
		if (Sample_f(tspack, w, &wi, samples[2*i], samples[2*i+1], &f, &pdf) && pdf > 0.) 
			r.AddWeighted(fabsf(wi.z) / pdf, f);
	}
	return r / nSamples;
}
SWCSpectrum BxDF::rho(const TsPack *tspack, int nSamples, float *samples) const {
	if (!samples) {
		samples =
			(float *)alloca(4 * nSamples * sizeof(float));
		LatinHypercube(tspack, samples, nSamples, 4);
	}
	SWCSpectrum r = 0.;
	for (int i = 0; i < nSamples; ++i) {
		// Estimate one term of $\rho_{hh}$
		Vector wo, wi;
		wo = UniformSampleHemisphere(samples[4*i], samples[4*i+1]);
		float pdf_o = INV_TWOPI, pdf_i = 0.f;
		SWCSpectrum f(0.f);			
		if (Sample_f(tspack, wo, &wi, samples[4*i+2], samples[4*i+3], &f, &pdf_i) && pdf_i > 0.)
			r.AddWeighted(fabsf(wi.z * wo.z) / (pdf_o * pdf_i), f);
	}
	return r / (M_PI*nSamples);
}
float BxDF::Weight(const TsPack *tspack, const Vector &wo, bool reverse) const
{
	return 1.f;
}
bool BSDF::Sample_f(const TsPack *tspack, const Vector &woW, Vector *wiW,
	float u1, float u2, float u3, SWCSpectrum *const f, float *pdf,
	BxDFType flags, BxDFType *sampledType, float *pdfBack,
	bool reverse) const
{
	float weights[MAX_BxDFS];
	// Choose which _BxDF_ to sample
	Vector wo = WorldToLocal(woW);
	u_int matchingComps = 0;
	float totalWeight = 0.f;
	for (int i = 0; i < nBxDFs; ++i) {
		if (bxdfs[i]->MatchesFlags(flags)) {
			weights[i] = bxdfs[i]->Weight(tspack, wo, reverse);
			totalWeight += weights[i];
			++matchingComps;
		} else
			weights[i] = 0.f;
	}
	if (matchingComps == 0 || !(totalWeight > 0.f)) {
		*pdf = 0.f;
		if (pdfBack)
			*pdfBack = 0.f;
		return false;
	}
	u3 *= totalWeight;
	int which = 0;
	for (int i = 0; i < nBxDFs; ++i) {
		if (weights[i] > 0.f) {
			which = i;
			u3 -= weights[i];
			if (u3 < 0.f) {
				break;
			}
		}
	}
	BxDF *bxdf = bxdfs[which];
	BOOST_ASSERT(bxdf); // NOBOOK
	// Sample chosen _BxDF_
	Vector wi;
	*pdf = 0.f;
	if (pdfBack)
		*pdfBack = 0.f;
	if (!bxdf->Sample_f(tspack, wo, &wi, u1, u2, f, pdf, pdfBack, reverse))
		return false;
	if (sampledType) *sampledType = bxdf->type;
	*wiW = LocalToWorld(wi);
	*pdf *= weights[which];
	if (pdfBack)
		*pdfBack *= weights[which];
	// Compute overall PDF with all matching _BxDF_s
	if (!(bxdf->type & BSDF_SPECULAR) && matchingComps > 1) {
		for (int i = 0; i < nBxDFs; ++i) {
			if (i != which && weights[i] > 0.f) {
				*pdf += bxdfs[i]->Pdf(tspack, wo, wi) * weights[i];
				if (pdfBack)
					*pdfBack += bxdfs[i]->Pdf(tspack, wi, wo) * weights[i];
			}
		}
	}
	*pdf /= totalWeight;
	if (pdfBack)
		*pdfBack /= totalWeight;
	// Compute value of BSDF for sampled direction
	if (!(bxdf->type & BSDF_SPECULAR)) {
		if (Dot(*wiW, ng) * Dot(woW, ng) > 0)
			// ignore BTDFs
			flags = BxDFType(flags & ~BSDF_TRANSMISSION);
		else
			// ignore BRDFs
			flags = BxDFType(flags & ~BSDF_REFLECTION);
		for (int i = 0; i < nBxDFs; ++i) {
			if (i != which && bxdfs[i]->MatchesFlags(flags)) {
				if (reverse)
					bxdfs[i]->f(tspack, wi, wo, f);
				else
					bxdfs[i]->f(tspack, wo, wi, f);
			}
		}
	}
	return true;
}
float BSDF::Pdf(const TsPack *tspack, const Vector &woW, const Vector &wiW,
		BxDFType flags) const {
	if (nBxDFs == 0.) return 0.;
	Vector wo = WorldToLocal(woW), wi = WorldToLocal(wiW);
	float pdf = 0.f;
	float totalWeight = 0.f;
	for (int i = 0; i < nBxDFs; ++i)
		if (bxdfs[i]->MatchesFlags(flags)) {
			float weight = bxdfs[i]->Weight(tspack, wo, false);
			pdf += bxdfs[i]->Pdf(tspack, wo, wi) * weight;
			totalWeight += weight;
		}
	return totalWeight > 0.f ? pdf / totalWeight : 0.f;
}
BSDF::BSDF(const DifferentialGeometry &dg,
           const Normal &ngeom, float e)
	: dgShading(dg), eta(e) {
	ng = ngeom;
	nn = dgShading.nn;
	sn = Normalize(dgShading.dpdu);
	tn = Cross(nn, sn);
	nBxDFs = 0;
}
SWCSpectrum BSDF::f(const TsPack *tspack, const Vector &woW,
		const Vector &wiW, BxDFType flags) const {
	Vector wi = WorldToLocal(wiW), wo = WorldToLocal(woW);
	if (Dot(wiW, ng) * Dot(woW, ng) > 0)
		// ignore BTDFs
		flags = BxDFType(flags & ~BSDF_TRANSMISSION);
	else
		// ignore BRDFs
		flags = BxDFType(flags & ~BSDF_REFLECTION);
	SWCSpectrum f = 0.;
	for (int i = 0; i < nBxDFs; ++i)
		if (bxdfs[i]->MatchesFlags(flags))
			bxdfs[i]->f(tspack, wo, wi, &f);
	return f;
}
SWCSpectrum BSDF::rho(const TsPack *tspack, BxDFType flags) const {
	SWCSpectrum ret(0.);
	for (int i = 0; i < nBxDFs; ++i)
		if (bxdfs[i]->MatchesFlags(flags))
			ret += bxdfs[i]->rho(tspack);
	return ret;
}
SWCSpectrum BSDF::rho(const TsPack *tspack, const Vector &wo, BxDFType flags) const {
	SWCSpectrum ret(0.);
	for (int i = 0; i < nBxDFs; ++i)
		if (bxdfs[i]->MatchesFlags(flags))
			ret += bxdfs[i]->rho(tspack, wo);
	return ret;
}
