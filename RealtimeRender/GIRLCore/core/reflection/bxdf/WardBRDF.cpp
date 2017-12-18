/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   WardBRDF.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - WardBRDF.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#include "girl.h"
#include "bxdf.h"
#include "color.h"
#include "spectrum.h"
#include "WardBRDF.h"

using namespace girl;

void WardBRDF::f(const TsPack *tspack, const Vector &_wo, const Vector &_wi, SWCSpectrum *const f) const
{
   Vector wo = Vector(c * _wo.x - s * _wo.y, s * _wo.x + c * _wo.y, _wo.z);
   Vector wi = Vector(c * _wi.x - s * _wi.y, s * _wi.x + c * _wi.y, _wi.z);
  
   Vector wh = Normalize(wo + wi);

   float sinThetaH = SinTheta(wh);
   float cosThetaH = CosTheta(wh);

   float cosPhiH, sinPhiH;
   // Avoid divide-by-zero when calculating cosPhiH and sinPhiH
   if(sinThetaH == 0.)
   {
      cosPhiH = 0.;
      sinPhiH = 0.;
   }
   else
   {
      cosPhiH = wh.x / sinThetaH;
      sinPhiH = wh.y / sinThetaH;
   }

   float tanThetaH = sinThetaH / cosThetaH;
   float tan2ThetaH = tanThetaH * tanThetaH;

   float cosThetaO = (CosTheta(wo));
   float cosThetaI = (CosTheta(wi));

   float cos2PhiH = cosPhiH * cosPhiH;
   float sin2PhiH = sinPhiH * sinPhiH;

   *f += Ks * 1.f / sqrtf(cosThetaI*cosThetaO)  * exp( -tan2ThetaH * (cos2PhiH * INV_Ax2 + sin2PhiH * INV_Ay2 ) ) * INV_4PI_AxAy;
}

float WardBRDF::Pdf(const TsPack *tspack, const Vector &_wo, const Vector &_wi) const

{
   Vector wo = Vector(c * _wo.x - s * _wo.y, s * _wo.x + c * _wo.y, _wo.z);
   Vector wi = Vector(c * _wi.x - s * _wi.y, s * _wi.x + c * _wi.y, _wi.z);

   Vector wh = Normalize(wo + wi);

   float sinThetaH = SinTheta(wh);

   float cosPhiH, sinPhiH;
   // Avoid divide-by-zero when calculating cosPhiH and sinPhiH
   if(sinThetaH == 0.)
   {
      cosPhiH = 0.;
      sinPhiH = 0.;
   }
   else
   {
      cosPhiH = wh.x / sinThetaH;
      sinPhiH = wh.y / sinThetaH;
   }

   float cosThetaH = CosTheta(wh);
   float cosThetaO = max(0.f, CosTheta(wo));
   float cosThetaI = max(0.f, CosTheta(wi));

   float tanThetaH = sinThetaH / cosThetaH;

   return max(0.f, float(INV_4PI_AxAy * 1.0f / ( Dot(wi, wh) * cosThetaH*cosThetaH*cosThetaH ) * exp( - tanThetaH*tanThetaH * (INV_Ax2 * cosPhiH*cosPhiH + INV_Ay2 * sinPhiH * sinPhiH)) ));
}

bool WardBRDF::Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi,
					  float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack,
					  bool reverse) const
{
   float phih;

   if( u1 < .25f )
      phih = atanf(Ay/Ax*tanf(2.f*M_PI*u1));
   else if( u1 < .5f )
      phih = M_PI-atanf(Ay/Ax*tanf(2.f*M_PI*(.5f-u1)));
   else if( u1 < .75f )
      phih = atanf(Ay/Ax*tanf(2.f*M_PI*(u1-.5f)))+M_PI;
   else
      phih = 2.f*M_PI-atanf(Ay/Ax*tanf(2.f*M_PI*(1.f-u1)));

   float cosphi = cosf(phih);
   float sinphi = sinf(phih);

   float thetah = atanf(sqrtf(-log(u2)/( cosphi*cosphi * INV_Ax2 + sinphi*sinphi * INV_Ay2 )));
   Vector h = SphericalDirection( sinf(thetah), cosf(thetah), phih );
   
   // Rotate h to follow gamma (we're rotating the other way here = +gamma => c = c, s=-s)
   h = Vector(c * h.x + s * h.y, -s * h.x + c * h.y, h.z);

   // normal flipping problem
   if( Dot( wo, h ) < 0.f )
      h = -h;

   *wi = 2 * Dot( wo, h ) * h - wo;

   *pdf = this->Pdf(tspack, wo, *wi);

   *f = SWCSpectrum(0.0);
   if (reverse) {
	   this->f(tspack, *wi, wo, f);
   }
   else
	   this->f(tspack, wo, *wi, f);

   return true;
}
