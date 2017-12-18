/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   roughglass.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - roughglass.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// roughglass.cpp*
#include "girl.h"
#include "material.h"

namespace girl
{

// RoughGlassGlass Class Declarations
class GIRLCOREDLL  RoughGlass : public Material {
public:
	// RoughGlass Public Methods
	RoughGlass(boost::shared_ptr<Texture<SWCSpectrum> > r, boost::shared_ptr<Texture<SWCSpectrum> > t, 
			boost::shared_ptr<Texture<float> > urough, boost::shared_ptr<Texture<float> > vrough,
			boost::shared_ptr<Texture<float> > i, boost::shared_ptr<Texture<float> > cbf, boost::shared_ptr<Texture<float> > bump) {
		Kr = r;
		Kt = t;
		uroughness = urough;
		vroughness = vrough;
		index = i;
		cauchyb = cbf;
		bumpMap = bump;
	}
	BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;
	
	static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);
private:
	// RoughGlass Private Data
	boost::shared_ptr<Texture<SWCSpectrum> > Kr, Kt;
	boost::shared_ptr<Texture<float> > index;
	boost::shared_ptr<Texture<float> > cauchyb;
	boost::shared_ptr<Texture<float> > uroughness;
	boost::shared_ptr<Texture<float> > vroughness;
	boost::shared_ptr<Texture<float> > bumpMap;
};

}//namespace girl

