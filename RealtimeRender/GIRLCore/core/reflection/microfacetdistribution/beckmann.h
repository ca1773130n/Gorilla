/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   beckmann.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - beckmann.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_BECKMANN_H
#define GIRL_BECKMANN_H
// beckmann.h*
#include "girl.h"
#include "microfacetdistribution.h"

namespace girl
{

class GIRLCOREDLL   Beckmann : public MicrofacetDistribution {
public:

  Beckmann (float rms);

  // Beckmann Public Methods
  float D (const Vector &wh) const;
  virtual void Sample_f (const Vector &wi, Vector *sampled_f, float u1, float u2, float *pdf) const;
  virtual float Pdf (const Vector &wi, const Vector &wo) const;

private:

  float r;
};

}//namespace girl

#endif // GIRL_BECKMANN_H

