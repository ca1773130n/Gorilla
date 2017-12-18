/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   LUNABXDF_H.h
//	Author	:   Sung-Soo Kim
//	Purpose :   LUNABXDF_H Material Definition
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2009-04-24  :   First version - LUNABXDF_H.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2009 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "girlmaterials.h"
#include "paramset.h"
#include "LunaBxDF.h"
#include "ParamConvertor.h"
#include "GirlTextureParams.h"

namespace girl
{

// LUNABXDF_H Class Declarations
class GIRLCOREDLL  LunaGirlMaterial : public Material {
public:
	// LunaGirlMaterial Public Methods
	LunaGirlMaterial(string shaderName, const TextureParams &mp, boost::shared_ptr<Texture<float> > bump) : bumpMap(bump)
	{
		m_strShader = shaderName;
	};

	LunaGirlMaterial(string shaderName, TP_GirlMaterial pTParams)
	{
		m_strShader = shaderName;
		m_TParams	= pTParams;
	};

	~LunaGirlMaterial(void) {}

	BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom,
	              const DifferentialGeometry &dgShading, float u) const;

	static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);

// LunaGirlMaterial Private Data
private:
	string			m_strShader;
	ParamSet		m_Params;
 	ParamConvertor	m_ParamConvertor;
	TP_GirlMaterial	m_TParams;
	LunaBxDFParamSet m_lunaParams;

	boost::shared_ptr<Texture<float>> bumpMap;
};

}//namespace girl

