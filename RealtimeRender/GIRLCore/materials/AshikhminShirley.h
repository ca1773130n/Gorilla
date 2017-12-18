/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   AshikhminShirley.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - AshikhminShirley.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_ASHIKHMINSHIRLEY_H
#define GIRL_ASHIKHMINSHIRLEY_H
// AshikhminShirley.cpp*
#include "girl.h"
#include "material.h"

namespace girl
{

	// AshikhminShirley Class Declarations
	class GIRLCOREDLL  AshikhminShirley : public Material {
	public:
		// AshikhminShirley Public Methods
		AshikhminShirley(boost::shared_ptr<Texture<SWCSpectrum> > kd, boost::shared_ptr<Texture<SWCSpectrum> > ks,
			boost::shared_ptr<Texture<float> > _f0,
			boost::shared_ptr<Texture<float> > _n,
			boost::shared_ptr<Texture<float> > bump) {
				Kd = kd;
				Ks = ks;
				f0 = _f0;
				n = _n;
				bumpMap = bump;
		}
		BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;

		static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);
	private:
		// AshikhminShirley Private Data
		boost::shared_ptr<Texture<SWCSpectrum> > Kd, Ks;
		boost::shared_ptr<Texture<float> > f0;
		boost::shared_ptr<Texture<float> > n;
		boost::shared_ptr<Texture<float> > bumpMap;

	};

}//namespace girl

#endif