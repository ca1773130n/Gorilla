/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   fresnel.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - fresnel.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// fresnel.cpp*
#include "fresnel.h"
#include "spectrum.h"

using namespace girl;

Fresnel::~Fresnel() { }

namespace girl
{

// Utility Functions
void FrDiel(float cosi, float cost,
				   const SWCSpectrum &etai,
				   const SWCSpectrum &etat,
				   SWCSpectrum *const f) {
	SWCSpectrum Rparl = (etat * cosi - etai * cost) /
						(etat * cosi + etai * cost);
	SWCSpectrum Rperp = (etai * cosi - etat * cost) /
						(etai * cosi + etat * cost);
	*f = (Rparl*Rparl + Rperp*Rperp) * 0.5f;
}
void FrCond(float cosi,
			const SWCSpectrum &eta,
			const SWCSpectrum &k,
			SWCSpectrum *const f) {
	SWCSpectrum tmp = (eta*eta + k*k) * (cosi*cosi) + 1;
	SWCSpectrum Rparl2 = 
		(tmp - (2.f * eta * cosi)) /
		(tmp + (2.f * eta * cosi));
	SWCSpectrum tmp_f = eta*eta + k*k + (cosi*cosi);
	SWCSpectrum Rperp2 =
		(tmp_f - (2.f * eta * cosi)) /
		(tmp_f + (2.f * eta * cosi));
	*f = (Rparl2 + Rperp2) * 0.5f;
}
SWCSpectrum FresnelApproxEta(const SWCSpectrum &Fr) {
	SWCSpectrum sqrtReflectance = Fr.Clamp(0.f, .999f).Sqrt();
	return (SWCSpectrum(1.) + sqrtReflectance) /
		(SWCSpectrum(1.) - sqrtReflectance);
}
 SWCSpectrum FresnelApproxK(const SWCSpectrum &Fr) {
	SWCSpectrum reflectance = Fr.Clamp(0.f, .999f);
	return 2.f * (reflectance /
		(SWCSpectrum(1.) - reflectance)).Sqrt();
}

}//namespace girl

