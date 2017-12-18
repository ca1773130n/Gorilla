/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   MerlHe.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - MerlHe.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#ifndef GIRL_WARDMODEL_H
#define GIRL_WARDMODEL_H

#include "girl.h"
#include "material.h"
#include "spectrum.h"
#include "HTSGBRDF.h"
#pragma warning( push )
#pragma warning (disable: 4005 ) //'M_PI' : macro redefinition
#include "MerlNganFit.h"
#pragma warning( pop )

namespace girl
{
class GIRLCOREDLL  MerlHe : public Material {
public:
   // MerlHe Public Methods
	MerlHe(
		const string &name,
		const string &refl_type,
		boost::shared_ptr<Texture<float> > bump);

   BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;

   static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);

private:
   // MerlHe Private Data
	boost::shared_ptr<Texture<float> > bumpMap;
	BxDFType reflection_type;
	string material_name;
	StrFMap p;
	float diffuse[3];
	float specular[3];
	float tau;
	float sigma;
	float n;
};
}; // end of namespace
#endif