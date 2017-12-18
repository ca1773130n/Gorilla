/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   wardisotropic.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - wardisotropic.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_WARDISOTROPIC_H
#define GIRL_WARDISOTROPIC_H
// wardisotropic.h*
#include "girl.h"
#include "microfacetdistribution.h"

namespace girl
{

// Kelemen and Szirmay-Kalos / Microfacet Based BRDF Model, Eurographics 2001
class GIRLCOREDLL   WardIsotropic : public MicrofacetDistribution {
public:

  WardIsotropic (float rms);

  // WardIsotropic Public Methods
  float D (const Vector &wh) const;
  virtual void Sample_f (const Vector &wi, Vector *sampled_f, float u1, float u2, float *pdf) const;
  virtual float Pdf (const Vector &wi, const Vector &wo) const;

private:

  float r;
};

}//namespace girl

#endif // GIRL_WARDISOTROPIC_H

