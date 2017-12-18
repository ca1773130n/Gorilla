/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   WardBRDF.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - WardBRDF.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#ifndef GIRL_WARDBXDF_H
#define GIRL_WARDBXDF_H

#include "girl.h"
#include "bxdf.h"
#include "spectrum.h"
#include "blinn.h"

namespace girl
{
	// Ward Class Declarations
	class GIRLCOREDLL  BlinnBRDF :public BxDF
	{
	public:
		BlinnBRDF(SWCSpectrum ks, float c, float r, Fresnel* f) : BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
			Ks(ks), eccentricity(c), rolloff(r) {
				distribution = new Blinn(eccentricity);
				fresnel = f;
		};
		~BlinnBRDF(){};
		void f(const TsPack *tspack, const Vector &_wo, const Vector &_wi, SWCSpectrum *const f) const;

		float G(const Vector &wo, const Vector &wi, const Vector &wh) const;
		float Pdf(const TsPack *tspack, const Vector &_wo, const Vector &_wi) const;
		bool Sample_f(const TsPack *tspack, const Vector &wo, Vector *wi,
			float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack = NULL,
			bool reverse = false) const;

	private:
		SWCSpectrum Ks;
		float eccentricity, rolloff;
		Blinn *distribution;
		Fresnel *fresnel;
	};
}; // end of namespace

#endif // GIRL_WARDBXDF_H