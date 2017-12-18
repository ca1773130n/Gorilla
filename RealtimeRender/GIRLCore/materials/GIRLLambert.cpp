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

// matte.cpp*
#include "GIRLLambert.h"
#include "bxdf.h"
#include "lambertian.h"
#include "orennayar.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// Matte Method Definitions
BSDF *GIRLLambert::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const 
{
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	//if (bumpMap)
		//Bump(bumpMap, dgGeom, dgShading, &dgs);
	//else
		dgs = dgShading;
	// Evaluate textures for _Matte_ material and allocate BRDF
	// NOTE - lordcrc - changed clamping to 0..1 to avoid >1 reflection
	SWCSpectrum r = color->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
	float sig = Clamp(sigma->Evaluate(tspack, dgs), 0.f, 90.f);
	BxDF *bxdf;
	if (sig == 0.)
		bxdf = BSDF_ALLOC(tspack, Lambertian)(r);
	else
		bxdf = BSDF_ALLOC(tspack, OrenNayar)(r, sig);
	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);

	// Add ptr to CompositingParams structure
	return bsdf;						 
}
Material* GIRLLambert::CreateMaterial(const Transform &xform, const TextureParams &mp) 
{
	boost::shared_ptr<Texture<SWCSpectrum> > color = mp.GetSWCSpectrumTexture("color", RGBColor(1.f));
	boost::shared_ptr<Texture<float> > sigma = mp.GetFloatTexture("sigma", 0.f);	

	// Get Compositing Params
	return new GIRLLambert(color, sigma, mp);									
}

static DynamicLoader::RegisterMaterial<GIRLLambert> r("GirlLambert");
