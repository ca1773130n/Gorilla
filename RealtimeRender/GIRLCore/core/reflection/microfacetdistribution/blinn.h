/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   blinn.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - blinn.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_BLINN_H
#define GIRL_BLINN_H
// blinn.h*
#include "girl.h"
#include "bxdf.h"
#include "geometry.h"
#include "microfacetdistribution.h"

namespace girl
{

class GIRLCOREDLL   Blinn : public MicrofacetDistribution {
public:
	Blinn(float e) { if (e > 100000.f || isnan(e)) e = 100000.f; exponent = e; }
	// Blinn Public Methods
	float D(const Vector &wh) const {
		float costhetah = fabsf(CosTheta(wh));
		return (exponent+2) *
		       INV_TWOPI *
			   powf(costhetah, exponent);
	}
	virtual void Sample_f(const Vector &wi, Vector *sampled_f, float u1, float u2, float *pdf) const;
	virtual float Pdf(const Vector &wi, const Vector &wo) const;
private:
	float exponent;
};

}//namespace girl

#endif // GIRL_BLINN_H

