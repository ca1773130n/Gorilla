#pragma once

#include "girl.h"
#include "material.h"

namespace girl
{
	enum enGIRLMaterialType 
	{
		GIRL_MATERIAL_BRDF_LAMBERT,
		GIRL_MATERIAL_BRDF_BLINN,
		GIRL_MATERIAL_BRDF_PHONG
	};

	class GIRLMaterial : public Material {
	public:
		GIRLMaterial( const TextureParams &mp ) 
		{ 
			SetCommonAttributes( mp );
		}
		~GIRLMaterial() 
		{ 
			//delete bumpMap.get();
			//delete color.get();
		}
		//virtual Material * CreateMaterial(const Transform &xform, const TextureParams &mp);
		void SetCommonAttributes( const TextureParams &mp );
		virtual BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom,
			const DifferentialGeometry &dgShading, float u) const = 0;

		enGIRLMaterialType GetGIRLMaterialType() { return girlMaterialType; }
		int GetGIRLMaterialTypeID() 
		{
			switch( girlMaterialType )
			{
			case GIRL_MATERIAL_BRDF_LAMBERT:
				return 0;
			case GIRL_MATERIAL_BRDF_BLINN:
				return 1;
			case GIRL_MATERIAL_BRDF_PHONG:
				return 2;
			default:
				return 0;
			}
		}

	public:
		enGIRLMaterialType girlMaterialType;
		string matteOpacityMode;

		bool refractions;
		bool hideSource;
		bool chromaticAberration;

		boost::shared_ptr<Texture<SWCSpectrum> > ambientColor;
		boost::shared_ptr<Texture<SWCSpectrum> > color;
		boost::shared_ptr<Texture<SWCSpectrum> > transparency;
		boost::shared_ptr<Texture<SWCSpectrum> > incandescence;
		boost::shared_ptr<Texture<SWCSpectrum> > bumpMap;
		
		boost::shared_ptr<Texture<float> > surfaceThickness;
		boost::shared_ptr<Texture<float> > refractionLimit;
		boost::shared_ptr<Texture<float> > translucenceFocus;
		boost::shared_ptr<Texture<float> > shadowAttenuation;
		boost::shared_ptr<Texture<float> > translucence;
		boost::shared_ptr<Texture<float> > refractiveIndex;
		boost::shared_ptr<Texture<float> > glowIntensity;
		boost::shared_ptr<Texture<float> > lightAbsorbance;
		boost::shared_ptr<Texture<float> > translucenceDepth;		
		boost::shared_ptr<Texture<float> > diffuse;
		boost::shared_ptr<Texture<float> > matteOpacity;
	};

}//namespace girl

