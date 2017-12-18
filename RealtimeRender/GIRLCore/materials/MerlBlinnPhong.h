/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   MerlBlinnPhong.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - MerlBlinnPhong.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_MERLBLINNPHONG_H
#define GIRL_MERLBLINNPHONG_H
// MerlBlinnPhong.cpp*
#include "girl.h"
#include "material.h"
#include "BlinnPhongBxDF.h"

#pragma warning( push )
#pragma warning (disable: 4005 ) //'M_PI' : macro redefinition
#include "MerlNganFit.h"
#pragma warning( pop )

namespace girl
{
	// MerlBlinnPhong Class Declarations
	class GIRLCOREDLL  MerlBlinnPhong : public Material {
	public:
		// MerlBlinnPhong Public Methods
		MerlBlinnPhong(
			const string &name,
			const string &refl_type,
			boost::shared_ptr<Texture<float> > bump);


		BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;

		static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);
	private:
		// MerlBlinnPhong Private Data
		boost::shared_ptr<Texture<float> > bumpMap;
		BxDFType reflection_type;
		string material_name;
		StrFMap p;
		float diffuse[3];
		float specular[3];
		float n;
	};

}//namespace girl

#endif // GIRL_MERLBLINNPHONG_H