/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   windy.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - windy.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// windy.cpp*
#include "windy.h"
#include "dynload.h"

using namespace girl;

static DynamicLoader::RegisterFloatTexture<WindyTexture<float> > r1("windy");
static DynamicLoader::RegisterSWCSpectrumTexture<WindyTexture<SWCSpectrum> > r2("windy");
