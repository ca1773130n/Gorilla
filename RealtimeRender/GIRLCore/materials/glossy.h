/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   glossy.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - glossy.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// glossy.cpp*
#include "girl.h"
#include "material.h"

namespace girl
{

// Glossy Class Declarations
class GIRLCOREDLL  Glossy : public Material {
public:
	// Glossy Public Methods
	Glossy(boost::shared_ptr<Texture<SWCSpectrum> > kd, boost::shared_ptr<Texture<SWCSpectrum> > ks,
			boost::shared_ptr<Texture<SWCSpectrum> > ka, boost::shared_ptr<Texture<float> > d,
			boost::shared_ptr<Texture<float> > u, boost::shared_ptr<Texture<float> > v,
			boost::shared_ptr<Texture<float> > bump) {
		Kd = kd;
		Ks = ks;
		Ka = ka;
		depth = d;
		nu = u;
		nv = v;
		bumpMap = bump;
	}
	BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;
	
	static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);
private:
	// Glossy Private Data
	boost::shared_ptr<Texture<SWCSpectrum> > Kd, Ks, Ka;
	boost::shared_ptr<Texture<float> > depth;
	boost::shared_ptr<Texture<float> > nu, nv;
	boost::shared_ptr<Texture<float> > bumpMap;
};

}//namespace girl
