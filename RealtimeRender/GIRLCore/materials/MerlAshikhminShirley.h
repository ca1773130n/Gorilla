/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   MerlAshikhminShirley.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - MerlAshikhminShirley.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_ASHIKHMINSHIRLEY_H
#define GIRL_ASHIKHMINSHIRLEY_H
// MerlAshikhminShirley.cpp*
#include "girl.h"
#include "bxdf.h"
#include "material.h"

#pragma warning( push )
#pragma warning (disable: 4005 ) //'M_PI' : macro redefinition
#include "MerlNganFit.h"
#pragma warning( pop )

namespace girl
{
	// MerlAshikhminShirley Class Declarations
	class GIRLCOREDLL  MerlAshikhminShirley : public Material {
	public:
		// MerlAshikhminShirley Public Methods
		MerlAshikhminShirley(
			const string &name,
			const string &refl_type,
			boost::shared_ptr<Texture<float> > bump);

		BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;

		static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);
	private:
		// MerlAshikhminShirley Private Data
		boost::shared_ptr<Texture<float> > bumpMap;
		BxDFType reflection_type;
		string material_name;
		StrFMap p;
		float diffuse[3];
		float specular[3];
		float F0;
		float n;
	};

}//namespace girl

#endif