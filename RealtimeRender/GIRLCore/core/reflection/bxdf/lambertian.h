/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   lambertian.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - lambertian.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_LAMBERTIAN_H
#define GIRL_LAMBERTIAN_H
// lambertian.h*
#include "girl.h"
#include "bxdf.h"
#include "spectrum.h"

namespace girl
{

class GIRLCOREDLL   Lambertian : public BxDF {
public:
	// Lambertian Public Methods
	Lambertian(const SWCSpectrum &reflectance)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)),
		  R(reflectance), RoverPI(reflectance * INV_PI) {
	}
	void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const;
	SWCSpectrum rho(const TsPack *tspack, const Vector &, int, float *) const {
		return R;
	}
	SWCSpectrum rho(const TsPack *tspack, int, float *) const { return R; }
private:
	// Lambertian Private Data
	SWCSpectrum R, RoverPI;
};

}//namespace girl

#endif // GIRL_LAMBERTIAN_H

