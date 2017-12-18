/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   fresnelblend.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - fresnelblend.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_FRESNELBLEND_H
#define GIRL_FRESNELBLEND_H
// fresnelblend.h*
#include "girl.h"
#include "bxdf.h"
#include "spectrum.h"

namespace girl
{

class GIRLCOREDLL   FresnelBlend : public BxDF
{
public:
	// FresnelBlend Public Methods
	FresnelBlend(const SWCSpectrum &Rd,
	             const SWCSpectrum &Rs,
	             const SWCSpectrum &Alpha,
	             float d,
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
	// FresnelBlend Private Data
	SWCSpectrum Rd, Rs, Alpha;
	float depth;
	MicrofacetDistribution *distribution;
};

}//namespace girl

#endif // GIRL_FRESNELBLEND_H

