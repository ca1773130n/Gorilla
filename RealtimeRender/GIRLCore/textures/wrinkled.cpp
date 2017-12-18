/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   wrinkled.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - wrinkled.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// wrinkled.cpp*
#include "wrinkled.h"
#include "dynload.h"

using namespace girl;

static DynamicLoader::RegisterFloatTexture<WrinkledTexture<float> > r1("wrinkled");
static DynamicLoader::RegisterSWCSpectrumTexture<WrinkledTexture<SWCSpectrum> > r2("wrinkled");
