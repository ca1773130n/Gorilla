/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   WardDuerBRDF.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - WardDuerBRDF.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#ifndef GIRL_WARDDUERBXDF_H
#define GIRL_WARDDUERBXDF_H

#include "girl.h"
#include "bxdf.h"
#include "spectrum.h"

namespace girl
{
// Ward Class Declarations
class GIRLCOREDLL  WardDuerBRDF :public BxDF
{
public:
   WardDuerBRDF(const SWCSpectrum &ks, float ax, float ay, float gamma) : BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
      Ks(ks), Ax(ax), Ay(ay), Gamma(gamma) {
         INV_Ax2 = 1.0f / (ax * ax);
         INV_Ay2 = 1.0f / (ay * ay);
         INV_4PI_AxAy = 1.0f / ( 4.0f * M_PI * Ax * Ay );

         // Used to rotate wi and wo.
         // -gamma to make this equivalet to rotating the x and y axes clockwise around z
         s = sin(-gamma);
         c = cos(-gamma);
   };
   ~WardDuerBRDF(){};
   void f(const TsPack *tspack, const Vector &_wo, const Vector &_wi, SWCSpectrum *const f) const;

   float Pdf(const TsPack *tspack, const Vector &_wo, const Vector &_wi) const;
   bool Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi,
	   float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack = NULL,
	   bool reverse = false) const;

private:
   SWCSpectrum Ks;
   float INV_Ax2, INV_Ay2, INV_4PI_AxAy, Ax, Ay, Gamma, s, c;
   Vector x,y,n;
};
}; // end of namespace

#endif // GIRL_WARDDUERBXDF_H