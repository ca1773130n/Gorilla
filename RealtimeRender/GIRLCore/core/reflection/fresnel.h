/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   fresnel.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - fresnel.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_FRESNEL_H
#define GIRL_FRESNEL_H
// fresnel.h*
#include "girl.h"
#include "spectrum.h"

namespace girl
{

class GIRLCOREDLL   Fresnel {
public:
	// Fresnel Interface
	virtual ~Fresnel();
	virtual void Evaluate(const TsPack *tspack, float cosi, SWCSpectrum *const f) const = 0;
};

void FrDiel(float cosi, float cost,
	const SWCSpectrum &etai, const SWCSpectrum &etat, SWCSpectrum *const f);
void FrCond(float cosi, const SWCSpectrum &n, const SWCSpectrum &k, SWCSpectrum *const f);
SWCSpectrum FresnelApproxEta(const SWCSpectrum &intensity);
SWCSpectrum FresnelApproxK(const SWCSpectrum &intensity);

}//namespace girl

#endif // GIRL_FRESNEL_H

