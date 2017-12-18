/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   fresnelconductor.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - fresnelconductor.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_FRESNELCONDUCTOR_H
#define GIRL_FRESNELCONDUCTOR_H
// fresnelconductor.h*
#include "girl.h"
#include "fresnel.h"

namespace girl
{

class GIRLCOREDLL   FresnelConductor : public Fresnel {
public:
	// FresnelConductor Public Methods
	void Evaluate(const TsPack *tspack, float cosi, SWCSpectrum *const f) const;
	FresnelConductor(const SWCSpectrum &e, const SWCSpectrum &kk)
		: eta(e), k(kk) {
	}
private:
	// FresnelConductor Private Data
	SWCSpectrum eta, k;
};

}//namespace girl

#endif // GIRL_FRESNELCONDUCTOR_H

