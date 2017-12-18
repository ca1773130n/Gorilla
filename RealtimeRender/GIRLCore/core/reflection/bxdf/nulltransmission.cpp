/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   nulltransmission.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - nulltransmission.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// nulltransmission.cpp*
#include "nulltransmission.h"
#include "spectrum.h"

using namespace girl;

bool NullTransmission::Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi,
								float u1, float u2, SWCSpectrum *const f, float *pdf, 
								float *pdfBack,	bool reverse) const {
	*wi = -wo;
	*pdf = 1.f;
	if (pdfBack)
		*pdfBack = 1.f;
	*f = 1.f / fabsf(CosTheta(*wi));
	return true;
}

