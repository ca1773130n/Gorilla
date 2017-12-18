/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   merlbxdf.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - merlbxdf.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#ifndef PBRT_MERLBXDF_H
#define PBRT_MERLBXDF_H
// MerlBxDF.h*

#ifdef ORIGINAL_SOURCE
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
// Merl BxDF
//
#define MERL_BXDF_REF_TYPE BSDF_ALL_REFLECTION
//#define MERL_BXDF_REF_TYPE BSDF_REFLECTION | BSDF_SPECULAR
//#define MERL_BXDF_REF_TYPE BSDF_REFLECTION | BSDF_DIFFUSE

class GIRLCOREDLL  MerlBxDF : public BxDF {
public:
	// MerlBxDFDebug Public Methods
	~MerlBxDF() {}
	
	MerlBxDF(double* table)
		: BxDF(BxDFType(MERL_BXDF_REF_TYPE)),
		MerlBRDF_table(table)
	{
		fR = 1.0;
		fG = 1.0;
		fB = 1.0;
	} 

	MerlBxDF(double* table, BxDFType type)
		: BxDF(type), MerlBRDF_table(table)
	{
		fR = 1.0;
		fG = 1.0;
		fB = 1.0;
	} 

	MerlBxDF(double* table, BxDFType type, float _r, float _g, float _b)
		: BxDF(type), MerlBRDF_table(table), fR(_r), fG(_g), fB(_b)
	{

	} 

	MerlBxDF(double* table, const SWCSpectrum &reflectance)
		: BxDF(BxDFType(MERL_BXDF_REF_TYPE)),
		MerlBRDF_table(table),
		R(reflectance),
		RoverPI(reflectance * INV_PI)
	{
		fR = 1.0;
		fG = 1.0;
		fB = 1.0;
	}

//	Spectrum f(const Vector &wo, const Vector &wi) const;

	void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const;

	double* MerlBRDF_table;

	// RGB control factors.
	float fR, fG, fB;

private:
	// Lambertian Private Data
	//Spectrum R, RoverPI;
	SWCSpectrum R, RoverPI;
};
}; // end of namespace

#endif // PBRT_MERLBXDF_H