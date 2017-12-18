/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   PbrtBxDF.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - PbrtBxDF.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#ifndef PBRT_PBRTBXDF_H
#define PBRT_PBRTBXDF_H
// PbrtBxDF.h*

#if 0
#include "pbrt.h"
#include "reflection.h"
#endif

#include "girl.h"
#include "bxdf.h"
#include "color.h"
#include "spectrum.h"

namespace girl
{

//
// Pbrt BxDF
//

#define PBRT_BXDF_REF_TYPE BSDF_ALL_REFLECTION
//#define PBRT_BXDF_REF_TYPE BSDF_REFLECTION | BSDF_SPECULAR
//#define PBRT_BXDF_REF_TYPE BSDF_REFLECTION | BSDF_DIFFUSE

class GIRLCOREDLL  PbrtBxDF : public BxDF {
public:
	// PbrtBxDFDebug Public Methods
	~PbrtBxDF() {}
	
	PbrtBxDF(double* table)
		: BxDF(BxDFType(PBRT_BXDF_REF_TYPE)),
		PbrtBRDF_table(table)
	{

	} 

	PbrtBxDF(double* table, BxDFType type)
		: BxDF(type), PbrtBRDF_table(table)
	{

	} 

	PbrtBxDF(double* table, const SWCSpectrum &reflectance)
		: BxDF(BxDFType(PBRT_BXDF_REF_TYPE)),
		PbrtBRDF_table(table),
		R(reflectance),
		RoverPI(reflectance * INV_PI)
	{

	}

//	Spectrum f(const Vector &wo, const Vector &wi) const;
	void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const;


private:
	double* PbrtBRDF_table;

	// Lambertian Private Data
	SWCSpectrum R, RoverPI;


};
class GIRLCOREDLL  PbrtBxDFPdf : public BxDF {
public:
	// PbrtBxDFPdfDebug Public Methods
	~PbrtBxDFPdf() {}
	
	PbrtBxDFPdf(double* brdf, double* pdf, BxDFType type)
		: BxDF(type), PbrtBRDF_table(brdf), PbrtPDF_table(pdf)
	{

	} 

//	Spectrum f(const Vector &wo, const Vector &wi) const;
	void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const;
	float Pdf(const Vector &wo, const Vector &wi) const;

private:

	double* PbrtBRDF_table;
	double* PbrtPDF_table;

	// Lambertian Private Data
	SWCSpectrum R, RoverPI;


};
};

#endif // PBRT_PBRTBXDF_H