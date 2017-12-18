/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   nulltransmission.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - nulltransmission.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_NULLTRANSMISSION_H
#define GIRL_NULLTRANSMISSION_H
// nulltransmission.h*
#include "girl.h"
#include "bxdf.h"
#include "spectrum.h"

namespace girl
{

class GIRLCOREDLL   NullTransmission : public BxDF {
public:
	// NullTransmission Public Methods
	NullTransmission()
		: BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)) {}
	void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const {
		*f += wo == -wi ? SWCSpectrum(1.f / fabsf(CosTheta(wi))) : SWCSpectrum(0.);
	}
	bool Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi,
		float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack = NULL,
		bool reverse = false) const;
	float Pdf(const TsPack *tspack, const Vector &wo, const Vector &wi) const {
		return wo == -wi ? 1.f : 0.f;
	}
private:
	// NullTransmission Private Data
};

}//namespace girl

#endif // GIRL_NULLTRANSMISSION_H

