/////////////// GIRL (Global Ilummination Renderer Library) Materials //////////////////
//	FileName:   LunaShaderFactory.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2009-04-20  :   First version - LunaShaderFactory.h initial implementation
//					FactoryPatterns for LunaShader classes creation.
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#include "LunaShaderFactory.h"

using namespace girl;

ClunaShader* LunaShaderFactory::createLunaShader(string shaderName, ParamSet mp)
{
	if (shaderName.c_str() == "GirlLambert") {
		m_pShader = (ClunaLambert*) new ClunaLambert(m_ParamConv.convertLambertParam(mp));
	} else if (shaderName.c_str() == "GirlPhong") {
		m_pShader = (ClunaPhong*) new ClunaPhong(m_ParamConv.convertPhongParam(mp));
	} else if (shaderName.c_str() == "GirlPhongE") {
		m_pShader = (ClunaPhongE*) new ClunaPhongE(m_ParamConv.convertPhongEParam(mp));
	} else if (shaderName.c_str() == "GirlBlinn") {
		m_pShader = (ClunaBlinn*) new ClunaBlinn(m_ParamConv.convertBlinnParam(mp));
	}

	return m_pShader;
}
