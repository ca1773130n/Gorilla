/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   MerlLafortune.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - MerlLafortune.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////


// MerlLafortune.cpp*
#if 0
#include "pbrt.h"
#include "material.h"
#endif

#include "girl.h"
#include "bxdf.h"
#include "lafortune.h"
#include "material.h"

#pragma warning( push )
#pragma warning (disable: 4005 ) //'M_PI' : macro redefinition
#include "MerlNganFit.h"
#pragma warning( pop )

float diffuse[3];
float xy0[3];
float z0[3];
float e0[3];

namespace girl {

// MerlLafortune Class Declarations
class GIRLCOREDLL  MerlLafortune : public Material {
public:
	MerlLafortune(
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
