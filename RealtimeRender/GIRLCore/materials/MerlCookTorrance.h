/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   MerlCookTorrance.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - MerlCookTorrance.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////


// MerlCookTorrance.cpp*
#if 0
#include "pbrt.h"
#include "material.h"
#endif

#include "girl.h"
#include "bxdf.h"
#include "cooktorrance.h"
#include "beckmann.h"
#include "fresnelslick.h"
#include "material.h"

#pragma warning( push )
#pragma warning (disable: 4005 ) //'M_PI' : macro redefinition
#include "MerlNganFit.h"
#pragma warning( pop )

namespace girl {

float diffuse[3];
float specular[3];
float F0;
float m;

// MerlCookTorrance Class Declarations
class GIRLCOREDLL  MerlCookTorrance : public Material {
public:
	MerlCookTorrance(
		const string &name,
		const string &refl_type,
		boost::shared_ptr<Texture<float> > bump);

//	BSDF *GetBSDF(const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading) const;
	BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;

	static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);

private:
	boost::shared_ptr<Texture<float> > bumpMap;
	BxDFType reflection_type;
	string material_name;
	StrFMap p;
};
}; // end of namespace