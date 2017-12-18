/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   specularreflection.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - specularreflection.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_SPECULARREFLECTION_H
#define GIRL_SPECULARREFLECTION_H
// specularreflection.h*
#include "girl.h"
#include "bxdf.h"
#include "spectrum.h"

namespace girl
{

class GIRLCOREDLL   SpecularReflection : public BxDF {
public:
	// SpecularReflection Public Methods
	SpecularReflection(const SWCSpectrum &r, Fresnel *f, float flm, float flmindex)
		: BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)),
		  R(r), fresnel(f), film(flm), filmindex(flmindex) {
	}
	void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const {
		
	}
	bool Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi,
		float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack = NULL, bool reverse = false) const;
	virtual float Weight(const TsPack *tspack, const Vector &wo, bool reverse) const;
	float Pdf(const TsPack *tspack, const Vector &wo, const Vector &wi) const {
		return 0.;
	}
private:
	// SpecularReflection Private Data
	SWCSpectrum R;
	Fresnel *fresnel;
	float film, filmindex;
};

class GIRLCOREDLL  ArchitecturalReflection : public SpecularReflection {
public:
	ArchitecturalReflection(const SWCSpectrum &r, Fresnel *f, float flm, float flmindex)
		: SpecularReflection(r, f, flm, flmindex) {}
	bool Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi,
		float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack = NULL, bool reverse = false) const;
	float Weight(const TsPack *tspack, const Vector &wo, bool reverse) const;
};

}//namespace girl

#endif // GIRL_SPECULARREFLECTION_H

