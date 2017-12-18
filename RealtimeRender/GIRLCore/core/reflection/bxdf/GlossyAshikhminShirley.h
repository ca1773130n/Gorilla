/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   GlossyAshikhminShirley.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - GlossyAshikhminShirley.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_ASHIKHMIN_H
#define GIRL_ASHIKHMIN_H
// fresnelblend.h*
#include "girl.h"
#include "bxdf.h"
#include "spectrum.h"

namespace girl
{

class GIRLCOREDLL   GlossyAshikhminShirley : public BxDF
{
public:
	// GlossyAshikhminShirley Public Methods
	GlossyAshikhminShirley(const SWCSpectrum &Rd,
	             const SWCSpectrum &Rs,
				 float f0,
				 MicrofacetDistribution *dist);
	void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const;
	SWCSpectrum SchlickFresnel(float costheta) const {
		return
			Rs + powf(1 - costheta, 5.f) * (SWCSpectrum(1.) - Rs);
	}
	bool Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi,
		float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack = NULL,
		bool reverse = false) const;
	float Pdf(const TsPack *tspack, const Vector &wi, const Vector &wo) const;
	
private:
	// GlossyAshikhminShirley Private Data
	SWCSpectrum Rd, Rs;
	float F0;
	MicrofacetDistribution *distribution;
	Fresnel **fresnel;
};

}//namespace girl

#endif // GIRL_ASHIKHMIN_H

