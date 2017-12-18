/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   mirror.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - mirror.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// mirror.cpp*
#include "girl.h"
#include "material.h"

namespace girl
{

// Mirror Class Declarations
class GIRLCOREDLL  Mirror : public Material {
public:
	// Mirror Public Methods
	Mirror(boost::shared_ptr<Texture<SWCSpectrum> > r, 
		boost::shared_ptr<Texture<float> > flm, boost::shared_ptr<Texture<float> > flmindex, 
		boost::shared_ptr<Texture<float> > bump) {
		Kr = r;
		film = flm;
		filmindex = flmindex;
		bumpMap = bump;
	}
	BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;
	
	static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);
private:
	// Mirror Private Data
	boost::shared_ptr<Texture<SWCSpectrum> > Kr;
	boost::shared_ptr<Texture<float> > film, filmindex;
	boost::shared_ptr<Texture<float> > bumpMap;
};

}//namespace girl
