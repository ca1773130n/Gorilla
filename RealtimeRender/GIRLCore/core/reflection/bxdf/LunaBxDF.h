/////////////// GIRL (Global Ilummination Renderer Library) Materials //////////////////
//	FileName:   LunaBxDF.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2009-04-20  :   First version - merlbxdf.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#ifndef LUNABXDF_H
#define LUNABXDF_H
// LunaBxDF.h*

#include "girl.h"
#include "girlmaterials.h"
#include "bxdf.h"
#include "color.h"
#include "spectrum.h"
#include "lunaShader.h"
#include "LunaShaderFactory.h"

namespace girl
{
//
// Luna BxDF
//
#define MERL_BXDF_REF_TYPE BSDF_ALL_REFLECTION

// vector type conversion function.
inline Vector3f	convertVector3f(Vector orgVector)
{
	Vector3f retVector;

	retVector[0] = orgVector[0];
	retVector[1] = orgVector[1];
	retVector[2] = orgVector[2];

	return retVector;
};

// vector type conversion function.
inline Vector3f	convertVector3f(const Normal &orgVector)
{
	Vector3f retVector;

	retVector[0] = orgVector[0];
	retVector[1] = orgVector[1];
	retVector[2] = orgVector[2];

	return retVector;
};

class GIRLCOREDLL  LunaBxDF : public BxDF {
public:
	// MerlBxDFDebug Public Methods
	~LunaBxDF() {
		if (m_pLunaShader)
			delete m_pLunaShader;
	}
	
	// TO DO: to create LunaShader class instance using Factory method pattern. -sungsoo
	LunaBxDF(string shaderName, ParamSet mp):BxDF(type), m_pLunaShader(NULL)
	{	
		if (shaderName == "GirlLambert")
			m_pLunaShader = (ClunaLambert*) new ClunaLambert(m_paramConv.convertLambertParam(mp));
		else if (shaderName == "GirlBlinn")
			m_pLunaShader = (ClunaBlinn*) new ClunaBlinn(m_paramConv.convertBlinnParam(mp));
		else if (shaderName == "GirlPhong")
			m_pLunaShader = (ClunaPhong*) new ClunaPhong(m_paramConv.convertPhongParam(mp));
		else if (shaderName == "GirlPhongE")
			m_pLunaShader = (ClunaPhongE*) new ClunaPhongE(m_paramConv.convertPhongEParam(mp));

	}

	LunaBxDF(string shaderName, const Normal &ngeom, LunaBxDFParamSet &lp):BxDF(type), m_pLunaShader(NULL)
	{
		Vector3f normalVec = convertVector3f(ngeom);

		if (shaderName == "GirlLambert") {
			m_pLunaShader = (ClunaLambert*) new ClunaLambert(m_paramConv.convertLambertParam(lp), normalVec);
		} else if (shaderName == "GirlBlinn") {
			m_pLunaShader = (ClunaLambert*) new ClunaBlinn(m_paramConv.convertBlinnParam(lp), normalVec);
		} else if (shaderName == "GirlPhong") {
			m_pLunaShader = (ClunaLambert*) new ClunaPhong(m_paramConv.convertPhongParam(lp), normalVec);
		} else if (shaderName == "GirlPhongE") {
			m_pLunaShader = (ClunaLambert*) new ClunaPhongE(m_paramConv.convertPhongEParam(lp), normalVec);
		}
	}

	void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const;

private:
	// Lambertian Private Data
	ClunaShader*	m_pLunaShader;
	ParamConvertor	m_paramConv;
};
}; // end of namespace

#endif // LUNABXDF_H