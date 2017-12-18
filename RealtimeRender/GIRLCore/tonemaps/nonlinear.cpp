/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   nonlinear.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - nonlinear.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// nonlinear.cpp*
#include "nonlinear.h"
#include "dynload.h"

using namespace girl;

// NonLinearOp Method Definitions
ToneMap* NonLinearOp::CreateToneMap(const ParamSet &ps) {
	float maxy = ps.FindOneFloat("maxY", 0.f);
	return new NonLinearOp(maxy);
}

static DynamicLoader::RegisterToneMap<NonLinearOp> r("nonlinear");
