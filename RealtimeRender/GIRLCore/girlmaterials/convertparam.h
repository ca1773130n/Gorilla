/////////////// GIRL (Global Ilummination Renderer Library) Materials //////////////////
//	FileName:   convertparam.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2009-04-20  :   First version - convertparam.h initial implementation
//					DLL export/import directives
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "girl.h"
#include "girlmaterials.h"
#include "paramset.h"

using namespace girl;

void conParam2Lambert(const TextureParams &mp, stLambert &lParam);
void parseLambertParam(const TextureParams &mp, ParamSet &tParam);
ParamSet makeParamSet(string shaderName, const TextureParams &mp);
stLambert convertLambertParam(ParamSet ps);

