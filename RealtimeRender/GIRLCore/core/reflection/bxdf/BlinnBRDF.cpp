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
*   Girl Renderer website : http://www.luxrender.net                       *
***************************************************************************/

// BlinnBRDF.cpp*
#include "BlinnBRDF.h"
#include "spectrum.h"
#include "mc.h"
#include "microfacetdistribution.h"
#include "fresnel.h"

using namespace girl;

void BlinnBRDF::f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const {

	const float cosThetaO = fabsf(CosTheta(wo));
	const float cosThetaI = fabsf(CosTheta(wi));
	const Vector wh(Normalize(wi + wo));
	const float cosThetaH = Dot(wi, wh);
	const float cG = G(wo, wi, wh);

	SWCSpectrum F;
	fresnel->Evaluate(tspack, cosThetaH, &F);
	f->AddWeighted(distribution->D(wh) * cG  / cosThetaO, Ks * rolloff * F);
}

float BlinnBRDF::G(const Vector &wo, const Vector &wi, const Vector &wh) const
{
	const float NdotWh = fabsf(CosTheta(wh));
	const float NdotWo = fabsf(CosTheta(wo));
	const float NdotWi = fabsf(CosTheta(wi));
	const float WodotWh = AbsDot(wo, wh);
	return min(1.f, min((2.f * NdotWh * NdotWo / WodotWh), (2.f * NdotWh * NdotWi / WodotWh)));
}

bool BlinnBRDF::Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi, float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack, bool reverse) const
{
	distribution->Sample_f(wo, wi, u1, u2, pdf);
	if (*pdf == 0.f) {
		if (pdfBack)
			*pdfBack = 0.f;
		return false;
	}
	if (pdfBack)
		*pdfBack = Pdf(tspack, *wi, wo);

	*f = SWCSpectrum(0.f);
	if (reverse)
		this->f(tspack, *wi, wo, f);
	else
		this->f(tspack, wo, *wi, f);
	return true;
}

float BlinnBRDF::Pdf(const TsPack *tspack, const Vector &wo, const Vector &wi) const
{
	if (!SameHemisphere(wo, wi))
		return 0.f;

	return distribution->Pdf(wo, wi);
}

