/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   speculartransmission.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - speculartransmission.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_SPECULARTRANSMISSION_H
#define GIRL_SPECULARTRANSMISSION_H
// speculartransmission.h*
#include "girl.h"
#include "bxdf.h"
#include "spectrum.h"
#include "fresneldielectric.h"

namespace girl
{

class GIRLCOREDLL   SpecularTransmission : public BxDF {
public:
	// SpecularTransmission Public Methods
	SpecularTransmission(const SWCSpectrum &t, float ei, float et, float cbf, bool archi = false)
		: BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)),
		  T(t), etai(ei), etat(et), cb(cbf), architectural(archi),
		  fresnel(ei, et) {}
	void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const;
	virtual bool Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi,
		float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack = NULL, bool reverse = false) const;
	float Weight(const TsPack *tspack, const Vector &wo, bool reverse) const;
	float Pdf(const TsPack *tspack, const Vector &wo, const Vector &wi) const {
		if (architectural && wi == -wo) return 1.f;
		else return 0.f;
	}
private:
	// SpecularTransmission Private Data
	SWCSpectrum T;
	float etai, etat, cb;
	bool architectural;
	FresnelDielectric fresnel;
};

}//namespace girl

#endif // GIRL_SPECULARTRANSMISSION_H

