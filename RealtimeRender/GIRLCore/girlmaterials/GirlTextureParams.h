/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   GirlTextureParams.h
//	Author	:   Sung-Soo Kim
//	Purpose :   Texture Parameter Structure for LunaGirlMaterial
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2009-06-17  :   First version - GirlTextureParams.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2009 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "girl.h"
#include "girlmaterials.h"

namespace girl {
	struct TP_GirlMaterial
	{
		// super structure
		string shadername;	
		
		// Lambert
		boost::shared_ptr<Texture<SWCSpectrum>>	color;
		boost::shared_ptr<Texture<SWCSpectrum>> transparency;	
		boost::shared_ptr<Texture<SWCSpectrum>> ambientColor;	
		boost::shared_ptr<Texture<SWCSpectrum>> incandescence;	
		boost::shared_ptr<Texture<float>>		diffuse;			
		boost::shared_ptr<Texture<float>>		translucence;		
		boost::shared_ptr<Texture<float>>		translucenceDepth; 
		boost::shared_ptr<Texture<float>>		translucenceFocus; 
		boost::shared_ptr<Texture<float>>		glowIntensity;		
		string									matteOpacityMode;	
		boost::shared_ptr<Texture<float>>		matteOpacity;		
		bool									refractions;		
		bool									chromaticAberration;
		bool									hideSource;			
		boost::shared_ptr<Texture<float>>		refractiveIndex;	
		boost::shared_ptr<Texture<float>>		refractionLimit;	
		boost::shared_ptr<Texture<float>>		lightAbsorbance;	
		boost::shared_ptr<Texture<float>>		surfaceThickness;	
		boost::shared_ptr<Texture<float>>		shadowAttenuation;	

		// Reflect
		int										reflectionLimit;			
		boost::shared_ptr<Texture<SWCSpectrum>>	specularColor;			
		boost::shared_ptr<Texture<float>>		reflectivity;			
		boost::shared_ptr<Texture<SWCSpectrum>>	reflectedColor;			
		boost::shared_ptr<Texture<float>>		reflectionSpecularity;	

		// Blinn
		boost::shared_ptr<Texture<float>>		eccentricity;		
		boost::shared_ptr<Texture<float>>		specularRollOff;		
		bool									reflectionRollOff;	

		// Phong
		boost::shared_ptr<Texture<float>>		cosinePower;		

		// PhongE
		boost::shared_ptr<Texture<float>>		roughness;		
		boost::shared_ptr<Texture<float>>		highlightSize;	
		boost::shared_ptr<Texture<SWCSpectrum>>	whiteness;		
	};

	struct LunaBxDFParamSet
	{
		// super structure
		string shadername;	

		RGBColor color;				
		RGBColor transparency;		
		RGBColor ambientColor;		
		RGBColor incandescence;		
		float		diffuse;				
		float		translucence;		
		float		translucenceDepth;	
		float		translucenceFocus;	
		float		glowIntensity;		
		string		matteOpacityMode;	
		float		matteOpacity;		
		bool		refractions;			
		bool		chromaticAberration;	
		bool		hideSource;			
		float		refractiveIndex;		
		float		refractionLimit;		
		float		lightAbsorbance;		
		float		surfaceThickness;
		float		shadowAttenuation;

		int			reflectionLimit;		
		RGBColor	specularColor;		
		float		reflectivity;		
		RGBColor	reflectedColor;		
		float		reflectionSpecularity;

		float	eccentricity;		
		float	specularRollOff;		
		bool	reflectionRollOff;

		float		cosinePower;

		float		roughness;		
		float		highlightSize;	
		RGBColor	whiteness;		
	};
}// end of namespace
