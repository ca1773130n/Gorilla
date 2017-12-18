/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   maxwhite.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - maxwhite.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// maxwhite.cpp*
#include "maxwhite.h"
#include "dynload.h"

using namespace girl;

// MaxWhiteOp Method Definitions
ToneMap * MaxWhiteOp::CreateToneMap(const ParamSet &ps) {
	return new MaxWhiteOp;
}

static DynamicLoader::RegisterToneMap<MaxWhiteOp> r("maxwhite");
