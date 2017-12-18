/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   He.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - He.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#ifndef GIRL_WARDMODEL_H
#define GIRL_WARDMODEL_H

#include "girl.h"
#include "material.h"
#include "spectrum.h"
#include "HTSGBRDF.h"

namespace girl
{
class GIRLCOREDLL  He : public Material {
public:
   // He Public Methods
   He(boost::shared_ptr<Texture<SWCSpectrum> > kd,
      boost::shared_ptr<Texture<SWCSpectrum> > ks,
      boost::shared_ptr<Texture<float> > bump,
      boost::shared_ptr<Texture<float> > tauValue,
      boost::shared_ptr<Texture<float> > sigmaValue,
      boost::shared_ptr<Texture<float> > nrealValue,
	  boost::shared_ptr<Texture<float> > nimagValue
	  ) {
         Kd = kd;
         Ks = ks;
		 tau = tauValue;
		 sigma = sigmaValue;
		 nreal = nrealValue;
		 nimag = nimagValue;
   }

   BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;

   static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);

private:
   // He Private Data
   boost::shared_ptr<Texture<SWCSpectrum> > Kd, Ks;		// diffuse, specular
   boost::shared_ptr<Texture<float> > bumpMap;			// bump map
   boost::shared_ptr<Texture<float> > tau;				// auto-correction length (tau)
   boost::shared_ptr<Texture<float> > sigma;			// standard devation (sigma)
   boost::shared_ptr<Texture<float> > nreal;			// refractive index (n-real)
   boost::shared_ptr<Texture<float> > nimag;			// refractive index (n-imaginary)
};
}; // end of namespace
#endif