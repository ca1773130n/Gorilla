/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   MerlBRDF.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - MerlBRDF.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "MerlBRDF.h"
#include "paramset.h"
#include "dynload.h"

// MerlBRDF.cpp*
using namespace girl;

// MerlBRDF Method Definitions
MerlBRDF::MerlBRDF(const string &type,
				   const string &refl_type,
				   boost::shared_ptr<Texture<SWCSpectrum> > kd,
				   boost::shared_ptr<Texture<float> > sig,
				   boost::shared_ptr<Texture<float> > bump,
				   boost::shared_ptr<Texture<float> > r,
				   boost::shared_ptr<Texture<float> > g,
				   boost::shared_ptr<Texture<float> > b
				   ) 
{
	Kd = kd;
	sigma = sig;
	bumpMap = bump;
	fR = r;
	fG = g;
	fB = b;

	char MerlBRDF_filename[SIZE_BUF];
	char *MerlBRDF_path = getenv("BRDF_SEARCHPATH");
	sprintf(MerlBRDF_filename,"%s\\%s.binary", MerlBRDF_path, type.data());

	//
	// <task> read BRDF file
	//
	dataMap_iterator = dataMap.find(string(MerlBRDF_filename));
	if ( dataMap_iterator == dataMap.end() )
	{
		// no previous load. Load newly.
		if ( !MerlBRDF_load( MerlBRDF_filename, MerlBRDF_table) ) 
		{
			fprintf(stderr, "Error reading %s\n", MerlBRDF_filename);
			exit(1);
		}
		else
		{
			//fprintf(stderr, "BRDF file loaded: %s\n", MerlBRDF_filename);
			dataMap.insert(pair<string, double *>((string)MerlBRDF_filename, MerlBRDF_table));
		}
	}
	else
	{
		//re-use the previous load.
		//fprintf(stderr, "Re-using the BRDF file loaded: %s\n", MerlBRDF_filename);
		MerlBRDF_table = dataMap_iterator->second;
	}

	if (refl_type == "all")				reflection_type = BSDF_ALL_REFLECTION;
	else if (refl_type == "diffuse")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE);
	else if (refl_type == "glossy")		reflection_type = BxDFType(BSDF_REFLECTION | BSDF_GLOSSY);
	else if (refl_type == "specular")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_SPECULAR);

	// assign RGB control factors.
	fR = r;
	fG = g;
	fB = b;

#if 0
	// This function tests the table load
	TestBRDFLoad();
#endif

}

#if 0
BSDF *MerlBRDF::GetBSDF(const DifferentialGeometry &dgGeom,
		const DifferentialGeometry &dgShading) const 
{
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;

	BSDF *bsdf = BSDF_ALLOC(BSDF)(dgs, dgGeom.nn);
	
#if 0
	Spectrum r = Kd->Evaluate(dgs).Clamp();
	bsdf->Add(BSDF_ALLOC(MerlBxDF)(MerlBRDF_table, r));
#else
	bsdf->Add(BSDF_ALLOC(MerlBxDF)(MerlBRDF_table, reflection_type));
#endif

	return bsdf;
}
#endif
BSDF *MerlBRDF::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const
{
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;

	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);

	float _r = fR->Evaluate(tspack, dgs);
	float _g = fG->Evaluate(tspack, dgs);
	float _b = fB->Evaluate(tspack, dgs);

#if 0
	Spectrum r = Kd->Evaluate(dgs).Clamp();
	bsdf->Add(BSDF_ALLOC(MerlBxDF)(MerlBRDF_table, r));
#else
	bsdf->Add(BSDF_ALLOC(tspack, MerlBxDF)(MerlBRDF_table, reflection_type));
#endif

	return bsdf;
}


void MerlBRDF::TestBRDFLoad()
{
	// Open test file
	FILE* fp = fopen("MerlBRDFTest.txt","w");
#ifdef _DEBUG
	if (!fp)
	{
		fprintf(stderr, "Error writing to \"MerlBRDFTest.txt\"");
		return;
	}
#endif

	// print out a 16x64x16x64 table table of BRDF values
	const int n = 16; // samples for theta
	const int m = 16; // samples for phi 
	double red,green,blue;
	double length;
	double theta_in, phi_in, theta_out, phi_out;
	for (int i = 0 ; i < n ; i++) 
	{
		theta_in = i * 0.5 * M_PI / n;
		for (int j = 1 ; j < 2 ; j++) // This loop is meaningless in isotropic BRDF
		{
			phi_in = j * 2.0 * M_PI / 4;
			for (int k = 0 ; k < n ; k++) 
			{
				theta_out = k * 0.5 * M_PI / n;
				for (int l = 0 ; l < m ; l++) 
				{
					phi_out = - M_PI + l * 2.0 * M_PI / m; // The interval has been changed from [0, 2Pi] to [-Pi, Pi]

					MerlBRDF_lookup(
						MerlBRDF_table, 
						theta_in, phi_in, 
						theta_out, phi_out, 
						red, green, blue);

					//fprintf(fp, "%3d %3d %3d %3d: %f %f %f\n", i, j, k, l, (float)red, (float)green, (float)blue);
					length = sqrt (red*red + green*green + blue*blue);
					fprintf(fp, "%f %f %f %f\n", theta_in, theta_out, phi_out, length);
				}
			}
	    }
	}

	fclose(fp);	
}

Material * MerlBRDF::CreateMaterial(const Transform &xform,
		const TextureParams &mp) {

	boost::shared_ptr<Texture<SWCSpectrum> > Kd = mp.GetSWCSpectrumTexture("Kd", RGBColor(1.f));
	boost::shared_ptr<Texture<float> > sigma = mp.GetFloatTexture("sigma", 0.f);
	boost::shared_ptr<Texture<float> > bump = mp.GetFloatTexture("bumpmap", 0.f);

	string material_type = mp.FindString("merltype");
	if (material_type.size() == 0 ) material_type = "gold-metallic-paint";

	string reflection_type = mp.FindString("reflection");
	if ( reflection_type != "all" && reflection_type != "diffuse" && reflection_type != "glossy") 
	{
		printf(">> Reflection type has been changed to ""all"" from %s.\n", reflection_type.c_str());
		reflection_type = "all";
	}
	//printf(">> Reflection Type: %s\n", reflection_type.c_str());

	boost::shared_ptr<Texture<float> > _r = mp.GetFloatTexture("r", 1.f);
	boost::shared_ptr<Texture<float> > _g = mp.GetFloatTexture("g", 1.f);
	boost::shared_ptr<Texture<float> > _b = mp.GetFloatTexture("b", 1.f);
	
	return new MerlBRDF(material_type, reflection_type, Kd, sigma, bump, _r, _g, _b);
}

static DynamicLoader::RegisterMaterial<MerlBRDF> r("MerlBRDF");