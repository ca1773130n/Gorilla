/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   fresnelconductor.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - fresnelconductor.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// fresnelconductor.cpp*
#include "fresnelconductor.h"
#include "spectrum.h"

using namespace girl;

void FresnelConductor::Evaluate(const TsPack *tspack, float cosi, SWCSpectrum *const f) const {
	FrCond(fabsf(cosi), eta, k, f);
}

