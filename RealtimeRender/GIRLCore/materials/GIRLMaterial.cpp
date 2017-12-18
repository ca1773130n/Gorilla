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
#include "GIRLMaterial.h"
#include "bxdf.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

void
GIRLMaterial::SetCommonAttributes( const TextureParams &mp )
{
	matteOpacityMode = mp.FindString( "matteOpacityMode" );

	refractions = mp.FindBool( "refractions", FALSE );
	hideSource = mp.FindBool( "hideSource", FALSE );
	chromaticAberration = mp.FindBool( "chromaticAberration", FALSE );

	incandescence = mp.GetSWCSpectrumTexture( "incandescence", RGBColor(0.f) );
	ambientColor = mp.GetSWCSpectrumTexture( "ambientColor", RGBColor(0.f) );
	color = mp.GetSWCSpectrumTexture( "color", RGBColor(0.f) );
	transparency = mp.GetSWCSpectrumTexture( "transparency", RGBColor(0.f) );
	bumpMap = mp.GetSWCSpectrumTexture( "bumpmap", RGBColor(0.f) );

	surfaceThickness = mp.GetFloatTexture( "surfaceThickness", 0 );
	refractionLimit = mp.GetFloatTexture( "refractionLimit", 0 );
	translucenceFocus = mp.GetFloatTexture( "translucenceFocus", 0 );
	shadowAttenuation = mp.GetFloatTexture( "shadowAttenuation", 0 );
	translucence = mp.GetFloatTexture( "translucence", 0 );
	refractiveIndex = mp.GetFloatTexture( "refractiveIndex", 0 );
	glowIntensity = mp.GetFloatTexture( "glowIntensity", 0 );
	lightAbsorbance = mp.GetFloatTexture( "lightAbsorbance", 0 );
	translucenceDepth = mp.GetFloatTexture( "translucenceDepth", 0 );	
	diffuse = mp.GetFloatTexture( "diffuse", 0 );
	matteOpacity = mp.GetFloatTexture( "matteOpacity", 0 );
}
