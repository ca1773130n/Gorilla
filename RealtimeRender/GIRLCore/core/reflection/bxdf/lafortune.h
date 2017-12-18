/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   lafortune.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - lafortune.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_LAFORTUNE_H
#define GIRL_LAFORTUNE_H
// lafortune.h*
#include "girl.h"
#include "bxdf.h"
#include "spectrum.h"

namespace girl
{

class GIRLCOREDLL   Lafortune : public BxDF {
public:
	// Lafortune Public Methods
	Lafortune(const SWCSpectrum &r, u_int nl,
	          const SWCSpectrum *x, const SWCSpectrum *y, const SWCSpectrum *z,
			  const SWCSpectrum *e, BxDFType t);
	void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const;
	bool Sample_f(const TsPack *tspack, const Vector &wi, Vector *sampled_f,
		float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack = NULL, bool reverse = false) const;
	float Pdf(const TsPack *tspack, const Vector &wi, const Vector &wo) const;
private:
	// Lafortune Private Data
	SWCSpectrum R;
	u_int nLobes;
	const SWCSpectrum *x, *y, *z, *exponent;
};

}//namespace girl

#endif // GIRL_LAFORTUNE_H

