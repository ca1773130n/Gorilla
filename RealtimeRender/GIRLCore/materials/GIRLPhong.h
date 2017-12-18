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

#include "girl.h"
#include "GIRLMaterial.h"

namespace girl
{

	// Matte Class Declarations
	class GIRLPhong : public GIRLMaterial {
	public:
		virtual ~GIRLPhong() {}
		// Matte Public Methods
		GIRLPhong( boost::shared_ptr<Texture<float> > cosinepower,
				   boost::shared_ptr<Texture<float> > reflectionspecularity,
				   boost::shared_ptr<Texture<float> > reflectionlimit,
				   boost::shared_ptr<Texture<float> > reflectivityvalue,
				   boost::shared_ptr<Texture<SWCSpectrum> > specularcolor,
				   boost::shared_ptr<Texture<SWCSpectrum> > reflectedcolor,
				   const TextureParams &mp 
				   ) : GIRLMaterial( mp )
		{
				girlMaterialType = GIRL_MATERIAL_BRDF_PHONG;
				cosinePower = cosinepower;
				reflectionSpecularity = reflectionspecularity;
				reflectionLimit = reflectionlimit;
				reflectivity = reflectivityvalue;
				reflectedColor = reflectedcolor;
				specularColor = specularcolor;
		}
		virtual BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;

		static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);

	public:
		boost::shared_ptr<Texture<SWCSpectrum> > specularColor;
		boost::shared_ptr<Texture<float> > cosinePower;
		boost::shared_ptr<Texture<float> > reflectionSpecularity;
		boost::shared_ptr<Texture<float> > reflectionLimit;
		boost::shared_ptr<Texture<float> > reflectivity;

		boost::shared_ptr<Texture<SWCSpectrum> > reflectedColor;
	};

}//namespace girl

