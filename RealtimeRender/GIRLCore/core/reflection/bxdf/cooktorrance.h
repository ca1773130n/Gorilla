/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   cooktorrance.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - cooktorrance.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_COOKTORRANCE_H
#define GIRL_COOKTORRANCE_H
// cooktorrance.h*
#include "girl.h"
#include "bxdf.h"
#include "spectrum.h"

namespace girl
{

// Multilobe Cook-Torrance model
class GIRLCOREDLL   CookTorrance : public BxDF {
public:
  // CookTorrance Public Methods
  CookTorrance(const SWCSpectrum &kd, u_int nl,
               const SWCSpectrum *ks, MicrofacetDistribution **dist, Fresnel **fres);
  //  [11/21/2008 Sung-Soo Kim]
  CookTorrance(const SWCSpectrum &kd, u_int nl,
	  const SWCSpectrum *ks, MicrofacetDistribution **dist, Fresnel **fres, BxDFType t);

  void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const;
  float G(const Vector &wo, const Vector &wi, const Vector &wh) const;
  bool Sample_f(const TsPack *tspack, const Vector &wi, Vector *sampled_f, float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack = NULL, bool reverse = false) const;
  float Pdf(const TsPack *tspack, const Vector &wi, const Vector &wo) const;
private:
  // Cook-Torrance Private Data
  SWCSpectrum KD;
  u_int nLobes;
  const SWCSpectrum *KS;
  MicrofacetDistribution **distribution;
  Fresnel **fresnel;
};

}//namespace girl

#endif // GIRL_COOKTORRANCE_H

