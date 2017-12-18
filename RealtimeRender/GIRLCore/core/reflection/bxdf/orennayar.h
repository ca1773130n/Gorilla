/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   orennayar.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - orennayar.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_ORENNAYAR_H
#define GIRL_ORENNAYAR_H
// orennayar.h*
#include "girl.h"
#include "bxdf.h"
#include "spectrum.h"

namespace girl
{

class GIRLCOREDLL   OrenNayar : public BxDF {
public:
	// OrenNayar Public Methods
	void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const;
	OrenNayar(const SWCSpectrum &reflectance, float sig)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)),
		  R(reflectance) {
		float sigma = Radians(sig);
		float sigma2 = sigma*sigma;
		A = 1.f - (sigma2 / (2.f * (sigma2 + 0.33f)));
		B = 0.45f * sigma2 / (sigma2 + 0.09f);
	}
private:
	// OrenNayar Private Data
	SWCSpectrum R;
	float A, B;
};

}//namespace girl

#endif // GIRL_ORENNAYAR_H

