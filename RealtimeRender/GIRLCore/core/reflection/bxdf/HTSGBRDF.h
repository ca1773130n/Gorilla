/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   HTSGBRDF.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - HTSGBRDF.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#ifndef GIRL_HTSGBXDF_H
#define GIRL_HTSGBXDF_H

#include "girl.h"
#include "bxdf.h"
#include "spectrum.h"
#include "util.h"
#include "mycomplex.h"

namespace girl
{
// HTSGBRDF Class Declarations
class GIRLCOREDLL  HTSGBRDF :public BxDF
{
public:
   HTSGBRDF(const SWCSpectrum &kd, const SWCSpectrum &ks, float t, float s, float _n) : BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
      Kd(kd), Ks(ks), tau(t), sigma(s), nreal(_n) {

   };

   HTSGBRDF(const SWCSpectrum &kd, const SWCSpectrum &ks, float t, float s, float _nreal, float _nimag) : BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
	   Kd(kd), Ks(ks), tau(t), sigma(s), n(_nreal, _nimag) {

   };

   ~HTSGBRDF(){};
   void f(const TsPack *tspack, const Vector &_wo, const Vector &_wi, SWCSpectrum *const f) const;

private:
   SWCSpectrum Kd, Ks;
   float tau, sigma, nreal, nimag;
   complex n;
};
}; // end of namespace

#endif // GIRL_HTSGBXDF_H
