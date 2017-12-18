/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   anisotropic.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - anisotropic.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_ANISOTROPIC_H
#define GIRL_ANISOTROPIC_H
// anisotropic.h*
#include "girl.h"
#include "microfacetdistribution.h"
#include "bxdf.h"
#include "geometry.h"

namespace girl
{

class GIRLCOREDLL   Anisotropic : public MicrofacetDistribution {
public:
	// Anisotropic Public Methods
	Anisotropic(float x, float y) { ex = x; ey = y;
		if (ex > 100000.f || isnan(ex)) ex = 100000.f;
		if (ey > 100000.f || isnan(ey)) ey = 100000.f;
	}
	float D(const Vector &wh) const {
		float costhetah = fabsf(CosTheta(wh));
		float e = (ex * wh.x * wh.x + ey * wh.y * wh.y) /
			(1.f - costhetah * costhetah);
		return sqrtf((ex+2)*(ey+2)) * INV_TWOPI * powf(costhetah, e);
	}
	void Sample_f(const Vector &wo, Vector *wi, float u1, float u2, float *pdf) const;
	float Pdf(const Vector &wo, const Vector &wi) const;
	void sampleFirstQuadrant(float u1, float u2, float *phi, float *costheta) const;
private:
	float ex, ey;
};

}//namespace girl

#endif // GIRL_ANISOTROPIC_H

