/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   LunaGirlMaterial.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   LunaGirlMaterial class implementation
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2009-04-24  :   First version - LunaGirlMaterial.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2009 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "LunaGirlMaterial.h"
#include "bxdf.h"
#include "paramset.h"
#include "dynload.h"
// #include "ParamConvertor.h"

using namespace girl;

// LunaGirlMaterial Method Definitions
BSDF *LunaGirlMaterial::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom,
		const DifferentialGeometry &dgShading, float u) const {
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;

	LunaBxDFParamSet lunaParams;
	lunaParams.shadername = m_strShader;
	lunaParams.color.FromXYZ((m_TParams.color->Evaluate(tspack, dgs).Clamp(0.f, 1.f)).ToXYZ(tspack));
	lunaParams.transparency.FromXYZ((m_TParams.transparency->Evaluate(tspack, dgs).Clamp(0.f, 1.f)).ToXYZ(tspack));
	lunaParams.ambientColor.FromXYZ((m_TParams.ambientColor->Evaluate(tspack, dgs).Clamp(0.f, 1.f)).ToXYZ(tspack));			
	lunaParams.incandescence.FromXYZ((m_TParams.incandescence->Evaluate(tspack, dgs).Clamp(0.f, 1.f)).ToXYZ(tspack));		
	lunaParams.diffuse				= m_TParams.diffuse->Evaluate(tspack, dgs);			
	lunaParams.translucence			= m_TParams.translucence->Evaluate(tspack, dgs);			
	lunaParams.translucenceDepth	= m_TParams.translucenceDepth->Evaluate(tspack, dgs);	
	lunaParams.translucenceFocus	= m_TParams.translucenceFocus->Evaluate(tspack, dgs);	
	lunaParams.glowIntensity		= m_TParams.glowIntensity->Evaluate(tspack, dgs);		
	lunaParams.matteOpacityMode		= m_TParams.matteOpacityMode;		
	lunaParams.matteOpacity			= m_TParams.matteOpacity->Evaluate(tspack, dgs);			
	lunaParams.refractions			= m_TParams.refractions;		
	lunaParams.chromaticAberration	= m_TParams.chromaticAberration;
	lunaParams.hideSource			= m_TParams.hideSource;			
	lunaParams.refractiveIndex		= m_TParams.refractiveIndex->Evaluate(tspack, dgs);		
	lunaParams.refractionLimit		= m_TParams.refractionLimit->Evaluate(tspack, dgs);		
	lunaParams.lightAbsorbance		= m_TParams.lightAbsorbance->Evaluate(tspack, dgs);		
	lunaParams.surfaceThickness		= m_TParams.surfaceThickness->Evaluate(tspack, dgs);

	if (lunaParams.shadername == "GirlBlinn"){
		// Reflect params
		lunaParams.reflectionLimit = m_TParams.reflectionLimit;
		lunaParams.specularColor.FromXYZ((m_TParams.specularColor->Evaluate(tspack, dgs).Clamp(0.f, 1.f)).ToXYZ(tspack));
		lunaParams.reflectivity = m_TParams.reflectivity->Evaluate(tspack, dgs);
		lunaParams.reflectedColor.FromXYZ((m_TParams.reflectedColor->Evaluate(tspack, dgs).Clamp(0.f, 1.f)).ToXYZ(tspack)); 
		lunaParams.reflectionSpecularity = m_TParams.reflectionSpecularity->Evaluate(tspack, dgs);

		// Blinn params
		lunaParams.eccentricity = m_TParams.eccentricity->Evaluate(tspack, dgs);
		lunaParams.specularRollOff = m_TParams.specularRollOff->Evaluate(tspack, dgs);
		lunaParams.reflectionRollOff = m_TParams.reflectionRollOff;

	} else if (lunaParams.shadername == "GirlPhong") {
		// Reflect params
		lunaParams.reflectionLimit = m_TParams.reflectionLimit;
		lunaParams.specularColor.FromXYZ((m_TParams.specularColor->Evaluate(tspack, dgs).Clamp(0.f, 1.f)).ToXYZ(tspack));
		lunaParams.reflectivity = m_TParams.reflectivity->Evaluate(tspack, dgs);
		lunaParams.reflectedColor.FromXYZ((m_TParams.reflectedColor->Evaluate(tspack, dgs).Clamp(0.f, 1.f)).ToXYZ(tspack)); 
		lunaParams.reflectionSpecularity = m_TParams.reflectionSpecularity->Evaluate(tspack, dgs);

		// Phong params
		lunaParams.cosinePower = m_TParams.cosinePower->Evaluate(tspack, dgs);
	} else if (lunaParams.shadername == "GirlPhongE") {
		// Reflect params
		lunaParams.reflectionLimit = m_TParams.reflectionLimit;
		lunaParams.specularColor.FromXYZ((m_TParams.specularColor->Evaluate(tspack, dgs).Clamp(0.f, 1.f)).ToXYZ(tspack));
		lunaParams.reflectivity = m_TParams.reflectivity->Evaluate(tspack, dgs);
		lunaParams.reflectedColor.FromXYZ((m_TParams.reflectedColor->Evaluate(tspack, dgs).Clamp(0.f, 1.f)).ToXYZ(tspack)); 
		lunaParams.reflectionSpecularity = m_TParams.reflectionSpecularity->Evaluate(tspack, dgs);
	
		// PhongE params
		lunaParams.roughness = m_TParams.roughness->Evaluate(tspack, dgs);
		lunaParams.highlightSize = m_TParams.highlightSize->Evaluate(tspack, dgs);
		lunaParams.whiteness.FromXYZ((m_TParams.reflectedColor->Evaluate(tspack, dgs).Clamp(0.f, 1.f)).ToXYZ(tspack));
	} 


	// Construct BSDF.
	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);

//  	bsdf->Add(BSDF_ALLOC(tspack, LunaBxDF)(m_strShader, m_Params));
	bsdf->Add(BSDF_ALLOC(tspack, LunaBxDF)(lunaParams.shadername, dgGeom.nn, lunaParams));

	return bsdf;
}

Material* LunaGirlMaterial::CreateMaterial(const Transform &xform, const TextureParams &mp) 
{
	ParamConvertor paramCon;

	string shader_type = mp.FindString("shadername");

	TP_GirlMaterial tpMaterial;
	tpMaterial.shadername = shader_type;
	tpMaterial = paramCon.makeGirlParamSet(shader_type, mp);

	boost::shared_ptr<Texture<float> > bumpMap = mp.GetFloatTexture("bumpmap");

	return new LunaGirlMaterial(shader_type, tpMaterial);
}

static DynamicLoader::RegisterMaterial<LunaGirlMaterial> r("LunaGirlMaterial");
