/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   MerlLafortune.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - MerlLafortune.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////


// MerlLafortune.cpp*
#include "MerlLafortune.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// MerlLafortune Method Definitions
MerlLafortune::MerlLafortune(
		const string &name,
		const string &refl_type,
		boost::shared_ptr<Texture<float> > bump) : bumpMap(bump) 
{ 
	material_name = name;

	if (refl_type == "all")				reflection_type = BSDF_ALL_REFLECTION;
	else if (refl_type == "diffuse")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE);
	else if (refl_type == "glossy")		reflection_type = BxDFType(BSDF_REFLECTION | BSDF_GLOSSY);
	else if (refl_type == "specular")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_SPECULAR);

	buildMap_Lafortune();

	p = param_Lafortune(material_name);

	diffuse[0] = p["d_r"]; 
	diffuse[1] = p["d_g"]; 
	diffuse[2] = p["d_b"]; 
	
	xy0[0] = p["xy_r"];
	xy0[1] = p["xy_g"];
	xy0[2] = p["xy_b"];

	z0[0] = p["z_r"];
	z0[1] = p["z_g"];
	z0[2] = p["z_b"];

	e0[0] = p["e_r"];
	e0[1] = p["e_g"];
	e0[2] = p["e_b"];
}

#if 0
BSDF *MerlLafortune::GetBSDF(const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading) const {
	// Declare MerlLafortune coefficients

#if 0 // gold-metallic-paint
	static float diffuse[3] = { 0.0601, 0.0298, 0.00487 }; // given value
	//static float diffuse[3] = { 0.0191304, 0.00948563, 0.00155017 }; // after "/M_PI"
	//static float diffuse[3] = {0.18881, 0.0936195, 0.0152996}; // after "*M_PI"
	//static float diffuse[3] = {0,0,0};
	static float xy0[3] =     {  -1.00881, -0.96526, -0.866779  };
	static float z0[3] =      {   0.908947, 0.869705, 0.780974 };
	static float e0[3] =      {  12.8, 12.8, 12.8   };
#elif 0 // gold-metallic-paint2
	static float diffuse[3] = {		0.066, 0.0474, 0.0238 }; // given value
	static float xy0[3] =     {		-1.00316, -0.992316, -0.971874  };
	static float z0[3] =      {		0.973907, 0.963381, 0.943534 };
	static float e0[3] =      {		27.4, 27.4, 27.4   };
#elif 0 // gold-paint
	static float diffuse[3] = {		0.173, 0.0838, 0.0202 }; // given value
	static float xy0[3] =     {		-0.958097, -0.934807, -0.883617 };
	static float z0[3] =      {		0.871143, 0.849968, 0.803423 };
	static float e0[3] =      {		14.3, 14.3, 14.3   };
#endif

	static Spectrum xy[1]	= { Spectrum(xy0), };
	static Spectrum z[1]	= { Spectrum(z0) };
	static Spectrum e[1]	= { Spectrum(e0) };

	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;

	BSDF *bsdf = BSDF_ALLOC(BSDF)(dgs, dgGeom.nn);
	bsdf->Add(BSDF_ALLOC(Lafortune)(Spectrum(diffuse), 1, xy, xy, z, e,
		reflection_type));

	return bsdf;
}
#endif

BSDF * MerlLafortune::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const
{
#if 0 // gold-metallic-paint
	static float diffuse[3] = { 0.0601, 0.0298, 0.00487 }; // given value
	//static float diffuse[3] = { 0.0191304, 0.00948563, 0.00155017 }; // after "/M_PI"
	//static float diffuse[3] = {0.18881, 0.0936195, 0.0152996}; // after "*M_PI"
	//static float diffuse[3] = {0,0,0};
	static float xy0[3] =     {  -1.00881, -0.96526, -0.866779  };
	static float z0[3] =      {   0.908947, 0.869705, 0.780974 };
	static float e0[3] =      {  12.8, 12.8, 12.8   };
#elif 0 // gold-metallic-paint2
	static float diffuse[3] = {		0.066, 0.0474, 0.0238 }; // given value
	static float xy0[3] =     {		-1.00316, -0.992316, -0.971874  };
	static float z0[3] =      {		0.973907, 0.963381, 0.943534 };
	static float e0[3] =      {		27.4, 27.4, 27.4   };
#elif 0 // gold-paint
	static float diffuse[3] = {		0.173, 0.0838, 0.0202 }; // given value
	static float xy0[3] =     {		-0.958097, -0.934807, -0.883617 };
	static float z0[3] =      {		0.871143, 0.849968, 0.803423 };
	static float e0[3] =      {		14.3, 14.3, 14.3   };
#endif

#if 0
	static SWCSpectrum xy[1]	= { SWCSpectrum(xy0), };
	static SWCSpectrum z[1]	= { SWCSpectrum(z0) };
	static SWCSpectrum e[1]	= { SWCSpectrum(e0) };
#endif
	RGBColor xyParam = RGBColor(xy0);
	RGBColor zParam = RGBColor(z0);
	RGBColor eParam = RGBColor(e0);

	static SWCSpectrum xy[1]	= { SWCSpectrum(tspack, xyParam)};
	static SWCSpectrum z[1]	= { SWCSpectrum(tspack, zParam) };
	static SWCSpectrum e[1]	= { SWCSpectrum(tspack, eParam) };


	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;

	RGBColor diffuseParam = RGBColor(diffuse);

	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);
	bsdf->Add(BSDF_ALLOC(tspack, Lafortune)(SWCSpectrum(tspack, diffuseParam), 1, xy, xy, z, e,
		reflection_type));

	return bsdf;
}


Material * MerlLafortune::CreateMaterial(const Transform &xform,
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

	return new MerlLafortune(material_name, reflection_type, bumpMap);
}

static DynamicLoader::RegisterMaterial<MerlLafortune> r("MerlLafortune");
static DynamicLoader::RegisterMaterial<MerlLafortune> r1("CookTorrance");

