/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   mixmaterial.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - mixmaterial.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// mixmaterial.cpp*
#include "mixmaterial.h"
#include "bxdf.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// MixMaterial Method Definitions
BSDF *MixMaterial::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const {
	float amt = amount->Evaluate(tspack, dgShading);
	if(u < amt)
		return child1->GetBSDF(tspack, dgGeom, dgShading, u / amt);
	else
		return child2->GetBSDF(tspack, dgGeom, dgShading, (u - amt) / (1.f - amt));
}
Material* MixMaterial::CreateMaterial(const Transform &xform,
		const TextureParams &mp) {
	string namedmaterial1 = mp.FindString("namedmaterial1"); // discarded as these are passed trough Context::Shape()
    string namedmaterial2 = mp.FindString("namedmaterial2"); // discarded as these are passed trough Context::Shape()
	boost::shared_ptr<Texture<float> > amount = mp.GetFloatTexture("amount", 0.5f);
	return new MixMaterial(amount);
}

static DynamicLoader::RegisterMaterial<MixMaterial> r("mix");
