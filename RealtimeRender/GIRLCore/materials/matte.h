/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   matte.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - matte.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// matte.cpp*
#include "girl.h"
#include "material.h"

namespace girl
{

// Matte Class Declarations
class GIRLCOREDLL  Matte : public Material {
public:
	// Matte Public Methods
	Matte(boost::shared_ptr<Texture<SWCSpectrum> > kd,
			boost::shared_ptr<Texture<float> > sig,
			boost::shared_ptr<Texture<float> > bump) {
		Kd = kd;
		sigma = sig;
		bumpMap = bump;
	}
	BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom,
	              const DifferentialGeometry &dgShading, float u) const;
	              
	static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);
private:
	// Matte Private Data
	boost::shared_ptr<Texture<SWCSpectrum> > Kd;
	boost::shared_ptr<Texture<float> > sigma, bumpMap;
};

}//namespace girl

