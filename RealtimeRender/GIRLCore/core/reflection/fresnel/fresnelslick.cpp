/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   fresnelslick.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - fresnelslick.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// fresnelslick.cpp*
#include "fresnelslick.h"
#include "spectrum.h"

using namespace girl;

void FresnelSlick::Evaluate(const TsPack *tspack, float cosi, SWCSpectrum *const f) const {
  *f = normal_incidence + (1.0f - normal_incidence) * powf (1.0 - cosi, 5.0f);
}

FresnelSlick::FresnelSlick (float ni) {
  normal_incidence = ni;
}

