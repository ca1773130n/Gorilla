/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   MerlCookTorrance.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - MerlCookTorrance.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// MerlCookTorrance.cpp*
#if 0
#include "pbrt.h"
#include "material.h"
#endif

#include "MerlCookTorrance.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// MerlCookTorrance Method Definitions
MerlCookTorrance::MerlCookTorrance(
		const string &name,
		const string &refl_type,
		boost::shared_ptr<Texture<float> > bump) : bumpMap(bump) 
{ 
	material_name = name;

	if (refl_type == "all")				reflection_type = BSDF_ALL_REFLECTION;
	else if (refl_type == "diffuse")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE);
	else if (refl_type == "glossy")		reflection_type = BxDFType(BSDF_REFLECTION | BSDF_GLOSSY);
	else if (refl_type == "specular")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_SPECULAR);

	buildMap_CookTorrance();

	p = param_CookTorrance(material_name);

	diffuse[0] = p["d_r"]; 
	diffuse[1] = p["d_g"]; 
	diffuse[2] = p["d_b"]; 
	
	specular[0] = p["s_r"]; 
	specular[1] = p["s_g"]; 
	specular[2] = p["s_b"]; 

	F0 = p["F0"];

	m = p["m"];

}

#if 0
BSDF *MerlCookTorrance::GetBSDF(const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading) const {
	// Declare MerlCookTorrance coefficients

#if 0 // gold-metallic-paint
	static float diffuse[3] =	{ 0.0657, 0.0312,0.00247 }; // given value
	static float specular[3] =	{ 0.231, 0.135, 0.0387  };
	static float F0 = 0.663;	// F(0)
	static float m	= 0.241;	// rms
#endif

	static Spectrum ks[1]	= { Spectrum(specular) };

	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;
	BSDF *bsdf = BSDF_ALLOC(BSDF)(dgs, dgGeom.nn);

	MicrofacetDistribution **D = (MicrofacetDistribution **)BSDF::Alloc(1 * sizeof(MicrofacetDistribution *));
	D[0] = BSDF_ALLOC(Beckmann)(m);


	Fresnel **F = (Fresnel **)BSDF::Alloc(1 * sizeof(Fresnel *));
	F[0] = BSDF_ALLOC(FresnelSchlick)(F0);


	bsdf->Add(BSDF_ALLOC(CookTorrance)(Spectrum(diffuse), 1, ks, D, F, reflection_type));

	return bsdf;
}
#endif
BSDF *MerlCookTorrance::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const
{
#if 0 // gold-metallic-paint
	static float diffuse[3] =	{ 0.0657, 0.0312,0.00247 }; // given value
	static float specular[3] =	{ 0.231, 0.135, 0.0387  };
	static float F0 = 0.663;	// F(0)
	static float m	= 0.241;	// rms
#endif

#if 0
	static SWCSpectrum ks[1]	= { SWCSpectrum(specular) };
#endif

	RGBColor specularParam = RGBColor(specular);
	static SWCSpectrum ks[1]	= { SWCSpectrum(tspack, specularParam) };

	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;
	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);

	MicrofacetDistribution **D = (MicrofacetDistribution **)BSDF::Alloc(tspack, 1 * sizeof(MicrofacetDistribution *));
	D[0] = BSDF_ALLOC(tspack, Beckmann)(m);


	Fresnel **F = (Fresnel **)BSDF::Alloc(tspack, 1 * sizeof(Fresnel *));
//	F[0] = BSDF_ALLOC(FresnelSchlick)(F0);
	F[0] = BSDF_ALLOC(tspack, FresnelSlick)(F0);

	RGBColor diffuseParam = RGBColor(diffuse);

	bsdf->Add(BSDF_ALLOC(tspack, CookTorrance)(SWCSpectrum(tspack, diffuseParam), 1, ks, D, F, reflection_type));

	return bsdf;

}

Material * MerlCookTorrance::CreateMaterial(const Transform &xform,
		const TextureParams &mp) {
	boost::shared_ptr<Texture<float> > bumpMap = mp.GetFloatTexture("bumpmap", 0.f);

	string material_name = mp.FindString("merltype");
	if (material_name.size() == 0 ) material_name = "gold-metallic-paint";

	string reflection_type = mp.FindString("reflection");
	if ( reflection_type != "all" && reflection_type != "diffuse" && reflection_type != "glossy") 
	{
		printf(">> Reflection type has been changed to ""all"" from %s.\n", reflection_type.c_str());
		reflection_type = "all";
	}

	return new MerlCookTorrance(material_name, reflection_type, bumpMap);
}

static DynamicLoader::RegisterMaterial<MerlCookTorrance> r("MerlCookTorrance");
static DynamicLoader::RegisterMaterial<MerlCookTorrance> r1("CookTorrance");
