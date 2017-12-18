/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   microfacetdistribution.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - microfacetdistribution.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_MICROFACETDISTRIBUTION_H
#define GIRL_MICROFACETDISTRIBUTION_H
// microfacetdistribution.h*
#include "girl.h"


namespace girl
{

class GIRLCOREDLL   MicrofacetDistribution {
public:
	// MicrofacetDistribution Interface
	virtual ~MicrofacetDistribution() { }
	virtual float D(const Vector &wh) const = 0;
	virtual void Sample_f(const Vector &wo, Vector *wi,
		float u1, float u2, float *pdf) const = 0;
	virtual float Pdf(const Vector &wo,
	                  const Vector &wi) const = 0;
};

}//namespace girl

#endif // GIRL_MICROFACETDISTRIBUTION_H

