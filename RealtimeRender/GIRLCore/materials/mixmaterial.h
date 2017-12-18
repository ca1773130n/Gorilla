/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   mixmaterial.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - mixmaterial.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// mixmaterial.cpp*
#include "girl.h"
#include "material.h"

namespace girl
{

// MixMaterial Class Declarations
class GIRLCOREDLL  MixMaterial : public Material {
public:
	// MixMaterial Public Methods
	MixMaterial(boost::shared_ptr<Texture<float> > a) {
		amount = a;
	}
	BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;
	
	static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);
private:
	// MixMaterial Private Data
	boost::shared_ptr<Texture<float> > amount;
};

}//namespace girl

