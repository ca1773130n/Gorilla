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
	class GIRLLambert : public GIRLMaterial {
	public:
		// Matte Public Methods
		GIRLLambert( boost::shared_ptr<Texture<SWCSpectrum> > col,
					 boost::shared_ptr<Texture<float> > sig,
					 const TextureParams &mp 
					 ) : GIRLMaterial( mp )			
		{
				girlMaterialType = GIRL_MATERIAL_BRDF_LAMBERT;
				color = col;
				sigma = sig;
				matname = "default";
		}
		virtual ~GIRLLambert() { }
		virtual BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;

		static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);

	public:
		// Matte Private Data
		boost::shared_ptr<Texture<float> > sigma;		
	};

}//namespace girl

