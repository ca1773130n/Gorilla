/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   glass.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - glass.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// glass.cpp*
#include "girl.h"
#include "material.h"

namespace girl
{

// Glass Class Declarations
class GIRLCOREDLL  Glass : public Material {
public:
	// Glass Public Methods
	Glass(boost::shared_ptr<Texture<SWCSpectrum> > r, boost::shared_ptr<Texture<SWCSpectrum> > t,
			boost::shared_ptr<Texture<float> > i, boost::shared_ptr<Texture<float> > cbf,
			boost::shared_ptr<Texture<float> > flm, boost::shared_ptr<Texture<float> > flmindex,
			bool archi, boost::shared_ptr<Texture<float> > bump) {
		Kr = r;
		Kt = t;
		index = i;
		cauchyb = cbf;
		film = flm;
		filmindex = flmindex;
		architectural = archi;
		bumpMap = bump;
	}
	BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;
	
	static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);
private:
	// Glass Private Data
	boost::shared_ptr<Texture<SWCSpectrum> > Kr, Kt;
	boost::shared_ptr<Texture<float> > index;
	boost::shared_ptr<Texture<float> > cauchyb;
	boost::shared_ptr<Texture<float> > film, filmindex;
	boost::shared_ptr<Texture<float> > bumpMap;
	bool architectural;
};

}//namespace girl

