/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   fresneldielectric.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - fresneldielectric.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_FRESNELDIELECTRIC_H
#define GIRL_FRESNELDIELECTRIC_H
// fresneldielectric.h*
#include "girl.h"
#include "fresnel.h"

namespace girl
{

class GIRLCOREDLL   FresnelDielectric : public Fresnel {
public:
	// FresnelDielectric Public Methods
	virtual void Evaluate(const TsPack *tspack, float cosi, SWCSpectrum *const f) const;
	FresnelDielectric(float ei, float et, float cB = 0.f) {
		eta_i = ei;
		eta_t = et;
		cb = cB;
	}
private:
	// FresnelDielectric Private Data
	float eta_i, eta_t, cb;
};

class GIRLCOREDLL  FresnelDielectricComplement : public FresnelDielectric {
public:
	void Evaluate(const TsPack *tspack, float cosi, SWCSpectrum *const f) const {
		FresnelDielectric::Evaluate(tspack, cosi, f);
		*f = SWCSpectrum(1.f) - *f;		
	}
	FresnelDielectricComplement(float ei, float et, float cB = 0.f) :
		FresnelDielectric(ei, et, cB) {}
};

}//namespace girl

#endif // GIRL_FRESNELDIELECTRIC_H

