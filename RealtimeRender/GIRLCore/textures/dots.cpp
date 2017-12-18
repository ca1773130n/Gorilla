/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   dots.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - dots.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// dots.cpp*
#include "dots.h"
#include "dynload.h"

using namespace girl;

static DynamicLoader::RegisterFloatTexture<DotsTexture<float> > r1("dots");
static DynamicLoader::RegisterSWCSpectrumTexture<DotsTexture<SWCSpectrum> > r2("dots");
