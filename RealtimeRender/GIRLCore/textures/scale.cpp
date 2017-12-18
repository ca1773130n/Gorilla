/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   scale.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - scale.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// scale.cpp*
#include "scale.h"
#include "dynload.h"

using namespace girl;

static DynamicLoader::RegisterFloatTexture<ScaleTexture<float, float> > r1("scale");
static DynamicLoader::RegisterSWCSpectrumTexture<ScaleTexture<SWCSpectrum, SWCSpectrum> > r2("scale");
