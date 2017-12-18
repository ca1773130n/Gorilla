/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   mix.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - mix.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// mix.cpp*
#include "mix.h"
#include "dynload.h"

using namespace girl;

static DynamicLoader::RegisterFloatTexture<MixTexture<float> > r1("mix");
static DynamicLoader::RegisterSWCSpectrumTexture<MixTexture<SWCSpectrum> > r2("mix");
