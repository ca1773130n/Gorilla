/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   mattetranslucent.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - mattetranslucent.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// mattetranslucent.cpp*
#include "girl.h"
#include "material.h"

namespace girl
{

// MatteTranslucent Class Declarations
class GIRLCOREDLL  MatteTranslucent : public Material {
public:
	// MatteTranslucent Public Methods
	MatteTranslucent(boost::shared_ptr<Texture<SWCSpectrum> > kr, boost::shared_ptr<Texture<SWCSpectrum> > kt,
			boost::shared_ptr<Texture<float> > sig,
			boost::shared_ptr<Texture<float> > bump) {
		Kr = kr;
		Kt = kt;
		sigma = sig;
		bumpMap = bump;
	}
	BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom,
	              const DifferentialGeometry &dgShading, float u) const;
	              
	static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);
private:
	// MatteTranslucent Private Data
	boost::shared_ptr<Texture<SWCSpectrum> > Kr, Kt;
	boost::shared_ptr<Texture<float> > sigma, bumpMap;
};

}//namespace girl

