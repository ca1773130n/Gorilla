/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   Ward.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - Ward.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#ifndef GIRL_WARDMODEL_H
#define GIRL_WARDMODEL_H

#include "girl.h"
#include "material.h"
#include "spectrum.h"
#include "WardBRDF.h"
#include "WardDuerBRDF.h"

namespace girl
{
class GIRLCOREDLL  Ward : public Material {
public:
   // Ward Public Methods
   Ward(boost::shared_ptr<Texture<SWCSpectrum> > kd,
      boost::shared_ptr<Texture<SWCSpectrum> > ks,
      boost::shared_ptr<Texture<float> > bump,
      boost::shared_ptr<Texture<float> > ax,
      boost::shared_ptr<Texture<float> > ay,
      boost::shared_ptr<Texture<float> > gamma,
	  bool _bWardDuer
      ) {
         Kd = kd;
         Ks = ks;
         bumpMap = bump;
         Ax = ax;
         Ay = ay;
         Gamma = gamma;
		 bWardDuer = _bWardDuer;
   }

   BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;

   static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);

private:
   // Ward Private Data
   boost::shared_ptr<Texture<SWCSpectrum> > Kd, Ks;
   boost::shared_ptr<Texture<float> > bumpMap;
   boost::shared_ptr<Texture<float> > Ax, Ay, Gamma;
   bool bWardDuer;

};
}; // end of namespace
#endif