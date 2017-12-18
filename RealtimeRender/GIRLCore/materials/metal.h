/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   metal.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - metal.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// metal.* - adapted to Girl from code by Asbjørn Heid
#include "girl.h"
#include "spd.h"
#include "material.h"

namespace girl
{

// Metal Class Declarations

class GIRLCOREDLL  Metal : public Material {
public:
	// Metal Public Methods
	Metal(boost::shared_ptr<SPD > n, boost::shared_ptr<SPD > k,
		boost::shared_ptr<Texture<float> > u, boost::shared_ptr<Texture<float> > v,
		boost::shared_ptr<Texture<float> > bump);

	BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, 
		const DifferentialGeometry &dgShading, float u) const;

	static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);

private:
	// Metal Private Data
	boost::shared_ptr<SPD > N, K;
	boost::shared_ptr<Texture<float> > nu, nv;
	boost::shared_ptr<Texture<float> > bumpMap;
};

static string DEFAULT_METAL = "aluminium";

}//namespace girl
