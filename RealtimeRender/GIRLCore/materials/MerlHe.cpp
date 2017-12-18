/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   MerlHe.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - MerlHe.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#include "bxdf.h"
#include "paramset.h"
#include "dynload.h"
#include "MerlHe.h"

using namespace girl;

// MerlHe Method Definitions
MerlHe::MerlHe(
	const string &name,
	const string &refl_type,
	boost::shared_ptr<Texture<float> > bump) : bumpMap(bump) 
{ 
	material_name = name;

	if (refl_type == "all")				reflection_type = BSDF_ALL_REFLECTION;
	else if (refl_type == "diffuse")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE);
	else if (refl_type == "glossy")		reflection_type = BxDFType(BSDF_REFLECTION | BSDF_GLOSSY);
	else if (refl_type == "specular")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_SPECULAR);

	buildMap_He();

	p = param_He(material_name);

	diffuse[0] = p["d_r"]; 
	diffuse[1] = p["d_g"]; 
	diffuse[2] = p["d_b"]; 

	specular[0] = p["s_r"]; 
	specular[1] = p["s_g"]; 
	specular[2] = p["s_b"]; 

	tau = p["tau"];
	sigma = p["sigma"];
	n = p["n"];
}

BSDF *MerlHe::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const {
	SWCSpectrum kd	= SWCSpectrum(tspack, RGBColor(diffuse));
	SWCSpectrum ks	= SWCSpectrum(tspack, RGBColor(specular));

	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;

	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);

	bsdf->Add(BSDF_ALLOC(tspack, HTSGBRDF)(kd, ks, tau, sigma, n, 0.0f));

	return bsdf;
}
// MerlHe Dynamic Creation Routine
Material * MerlHe::CreateMaterial(const Transform &xform,
                                    const TextureParams &mp) {
	string material_name = mp.FindString("merltype");
	if (material_name.size() == 0 ) material_name = "gold-metallic-paint";

	string reflection_type = mp.FindString("reflection");
	if ( reflection_type != "all" && reflection_type != "diffuse" && reflection_type != "glossy") 
	{
		printf(">> Reflection type has been changed to ""all"" from %s.\n", reflection_type.c_str());
		reflection_type = "all";
	}

	boost::shared_ptr<Texture<float> > bumpMap = mp.GetFloatTexture("bumpmap");

	return new MerlHe(material_name, reflection_type, bumpMap);
}

static DynamicLoader::RegisterMaterial<MerlHe> r("MerlHe");