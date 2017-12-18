/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   shinymetal.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - shinymetal.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// shinymetal.cpp*
#include "girl.h"
#include "material.h"

namespace girl
{

// ShinyMetal Class Declarations
class GIRLCOREDLL  ShinyMetal : public Material {
public:
	// ShinyMetal Public Methods
	ShinyMetal(boost::shared_ptr<Texture<SWCSpectrum> > ks,
		boost::shared_ptr<Texture<float> > u, boost::shared_ptr<Texture<float> > v,
		boost::shared_ptr<Texture<float> > flm, boost::shared_ptr<Texture<float> > flmindex, 
			boost::shared_ptr<Texture<SWCSpectrum> > kr, boost::shared_ptr<Texture<float> > bump) {
		Ks = ks;
		Kr = kr;
		nu = u;
		nv = v;
		film = flm;
		filmindex = flmindex;
		bumpMap = bump;
	}
	BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;
	
	static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);
private:
	// ShinyMetal Private Data
	boost::shared_ptr<Texture<SWCSpectrum> > Ks, Kr;
	boost::shared_ptr<Texture<float> > nu, nv;
	boost::shared_ptr<Texture<float> > film, filmindex;
	boost::shared_ptr<Texture<float> > bumpMap;
};

}//namespace girl

