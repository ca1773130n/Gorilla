/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   PbrtBRDF.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - PbrtBRDF.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
/*
 * PBRT BRDF shader for PBRT.
 *
 * Source code Copyright(c) 2007 Joo-Haeng Lee (joohaeng@etri.re.kr)
 *
 * All Rights Reserved.
 * For educational use only; commercial use expressly forbidden.
 * NO WARRANTY, express or implied, for this software.
 *
 * I do not provide MERL BRDF data files.
 * For the details including license of MERL BRDF, contact brdf@merl.com.
 * 
 */

// PbrtBRDF.cpp*
#include "PbrtBRDF.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// PbrtBRDF Method Definitions
PbrtBRDF::PbrtBRDF(const string &type,
				   const string &refl_type,
				   bool uniform,
//				   boost::shared_ptr<Texture<RGBColor> > kd,
				   boost::shared_ptr<Texture<SWCSpectrum> > kd,
				   boost::shared_ptr<Texture<float> > sig,
				   boost::shared_ptr<Texture<float> > bump) 
{
	Kd = kd;
	sigma = sig;
	bumpMap = bump;

	// read brdf
	char PbrtBRDF_filename[256];
	sprintf(PbrtBRDF_filename,"D:\\Dev\\pbrt\\brdfs\\%s.binary", type.data());
	if (!PbrtBRDF_load(PbrtBRDF_filename, PbrtBRDF_table)) 
	{
		fprintf(stderr, "Error reading %s\n", PbrtBRDF_filename);
		exit(1);
	}

	// read pdf
	if (uniform_pdf = uniform)
	{
		char PbrtPDF_filename[256];
		sprintf(PbrtPDF_filename,"D:\\Dev\\pbrt\\brdfs\\%s.pdf-binary", type.data());
		if (!PbrtPDF_load(PbrtPDF_filename, PbrtPDF_table)) 
		{
			fprintf(stderr, "Error reading %s\n", PbrtPDF_filename);
			exit(1);
		}
	}

	if (refl_type == "all")				reflection_type = BSDF_ALL_REFLECTION;
	else if (refl_type == "diffuse")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE);
	else if (refl_type == "glossy")		reflection_type = BxDFType(BSDF_REFLECTION | BSDF_GLOSSY);
	else if (refl_type == "specular")	reflection_type = BxDFType(BSDF_REFLECTION | BSDF_SPECULAR);


#if 0
	// This function tests the table load
	TestBRDFLoad();
#endif

}

#if 0
BSDF *PbrtBRDF::GetBSDF(const DifferentialGeometry &dgGeom,
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
	bsdf->Add(BSDF_ALLOC(PbrtBxDF)(PbrtBRDF_table, r));
#else
	if ( uniform_pdf )
	{
		bsdf->Add(BSDF_ALLOC(PbrtBxDF)(PbrtBRDF_table, reflection_type));
		//fprintf(stderr, ">> MSG: uniform pdf.\n");
	}
	else
	{
		bsdf->Add(BSDF_ALLOC(PbrtBxDFPdf)(PbrtBRDF_table, PbrtPDF_table, reflection_type));
		//fprintf(stderr, ">> MSG: non-uniform pdf.\n");
	}
#endif

	return bsdf;
}
#endif

BSDF *PbrtBRDF::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const
{
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;

	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);

#if 0
	Spectrum r = Kd->Evaluate(dgs).Clamp();
	bsdf->Add(BSDF_ALLOC(PbrtBxDF)(PbrtBRDF_table, r));
#else
	if ( uniform_pdf )
	{
		bsdf->Add(BSDF_ALLOC(tspack, PbrtBxDF)(PbrtBRDF_table, reflection_type));
		//fprintf(stderr, ">> MSG: uniform pdf.\n");
	}
	else
	{
		bsdf->Add(BSDF_ALLOC(tspack, PbrtBxDFPdf)(PbrtBRDF_table, PbrtPDF_table, reflection_type));
		//fprintf(stderr, ">> MSG: non-uniform pdf.\n");
	}
#endif

	return bsdf;
}


void PbrtBRDF::TestBRDFLoad()
{
	// Open test file
	FILE* fp = fopen("PbrtBRDFTest.txt","w");
#ifdef _DEBUG
	if (!fp)
	{
		fprintf(stderr, "Error writing to \"PbrtBRDFTest.txt\"");
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

					PbrtBRDF_lookup(
						PbrtBRDF_table, 
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


Material * PbrtBRDF::CreateMaterial(const Transform &xform,
		const TextureParams &mp) {

//	boost::shared_ptr<Texture<RGBColor> > Kd = mp.GetRGBColorTexture("Kd", RGBColor(1.f));
	boost::shared_ptr<Texture<SWCSpectrum> > Kd = mp.GetSWCSpectrumTexture("Kd", RGBColor(1.f));
	boost::shared_ptr<Texture<float> > sigma = mp.GetFloatTexture("sigma", 0.f);
	boost::shared_ptr<Texture<float> > bump = mp.GetFloatTexture("bumpmap", 0.f);

	string material_type = mp.FindString("merltype");
	if (material_type.size() == 0 ) material_type = "bluepaint";

	string reflection_type = mp.FindString("reflection");
	if ( reflection_type != "all" && reflection_type != "diffuse" && reflection_type != "glossy") 
	{
		printf(">> Reflection type has been changed to ""all"" from %s.\n", reflection_type.c_str());
		reflection_type = "all";
	}
	//printf(">> Reflection Type: %s\n", reflection_type.c_str());
	
	bool uniform_pdf = mp.FindBool("uniform", true);

	return new PbrtBRDF(material_type, reflection_type, uniform_pdf, Kd, sigma, bump);
}

static DynamicLoader::RegisterMaterial<PbrtBRDF> r("PbrtBRDF");