/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   fbm.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - fbm.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// fbm.cpp*
#include "fbm.h"
#include "dynload.h"

using namespace girl;

static DynamicLoader::RegisterFloatTexture<FBmTexture<float> > r1("fbm");
static DynamicLoader::RegisterSWCSpectrumTexture<FBmTexture<SWCSpectrum> > r2("fbm");
