/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   LunaBxDF.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2009-04-20  :   First version - LunaBxDF.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#include "LunaBxDF.h"

using namespace girl;

void LunaBxDF::f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const
{
	Vector3f retColor(1.0f);
	Vector3f outVec = convertVector3f(wo);
	Vector3f inVec = convertVector3f(wi);
	
	m_pLunaShader->compute(outVec, inVec, &retColor);

	float color[3] = {retColor[0], retColor[1], retColor[2]};
	RGBColor lunaColor(color);

	*f += SWCSpectrum(tspack,lunaColor);
}