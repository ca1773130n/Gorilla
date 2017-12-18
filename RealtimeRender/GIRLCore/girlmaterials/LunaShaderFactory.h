/////////////// GIRL (Global Ilummination Renderer Library) Materials //////////////////
//	FileName:   LunaShaderFactory.h
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
#pragma once

#include "girl.h"
#include "girlmaterials.h"
#include "paramset.h"
#include "ParamConvertor.h"

namespace girl
{
class LunaShaderFactory
{
public:
	LunaShaderFactory(void){};
	LunaShaderFactory(string name, ParamSet mp): m_pShader(NULL)
	{
		m_pShader = createLunaShader(name, mp);
	};
	~LunaShaderFactory(void){
		if (m_pShader)
			delete m_pShader;
	};

public:
	ClunaShader*	createLunaShader(string name, ParamSet mp);

private:
	ClunaShader*	m_pShader;
	ParamConvertor	m_ParamConv;
};

} // end of namespace
