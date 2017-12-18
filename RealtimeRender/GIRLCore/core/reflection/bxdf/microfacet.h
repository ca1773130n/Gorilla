/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   microfacet.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - microfacet.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_MICROFACET_H
#define GIRL_MICROFACET_H
// microfacet.h*
#include "girl.h"
#include "bxdf.h"
#include "spectrum.h"

namespace girl
{

class GIRLCOREDLL   Microfacet : public BxDF {
public:
	// Microfacet Public Methods
	Microfacet(const SWCSpectrum &reflectance, Fresnel *f,
		MicrofacetDistribution *d);
	void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const;
	float G(const Vector &wo, const Vector &wi,
			const Vector &wh) const {
		float NdotWh = fabsf(CosTheta(wh));
		float NdotWo = fabsf(CosTheta(wo));
		float NdotWi = fabsf(CosTheta(wi));
		float WOdotWh = AbsDot(wo, wh);
		return min(1.f, min((2.f * NdotWh * NdotWo / WOdotWh),
		                (2.f * NdotWh * NdotWi / WOdotWh)));
	}
	bool Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi,
		float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack = NULL,
		bool reverse = false) const;
	float Pdf(const TsPack *tspack, const Vector &wo, const Vector &wi) const;
private:
	// Microfacet Private Data
	SWCSpectrum R;
	MicrofacetDistribution *distribution;
	Fresnel *fresnel;
};


}//namespace girl

#endif // GIRL_MICROFACET_H

