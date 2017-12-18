/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   specularreflection.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - specularreflection.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// specularreflection.cpp*
#include "specularreflection.h"
#include "spectrum.h"
#include "spectrumwavelengths.h"
#include "fresnel.h"

using namespace girl;

// Compute reflectance weight for thin film interference with phase difference
inline void PhaseDifference(const TsPack *tspack, const Vector &wo, float film, float filmindex, SWCSpectrum *const Pd) {
	const float swo = SinTheta(wo);
	const float s = sqrtf(filmindex*filmindex-1.0f*1.0f*swo*swo);
	for(int i=0; i<WAVELENGTH_SAMPLES; i++) {
		const float pd = (4*M_PI*film/tspack->swl->w[i]) * s + M_PI;
		const float cpd = cosf(pd);
		Pd->c[i] *= cpd*cpd;
	}
}

bool SpecularReflection::Sample_f(const TsPack *tspack, const Vector &wo,
	Vector *wi, float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack, bool reverse) const {
	// Compute perfect specular reflection direction
	*wi = Vector(-wo.x, -wo.y, wo.z);
	*pdf = 1.f;
	if (pdfBack)
		*pdfBack = 1.f;
	fresnel->Evaluate(tspack, CosTheta(wo), f);
	if(film > 0.f) PhaseDifference(tspack, wo, film, filmindex, f);
	*f *= R;
	*f /= fabsf(CosTheta(wo));
	return true;
}
float SpecularReflection::Weight(const TsPack *tspack, const Vector &wo, bool reverse) const
{
	SWCSpectrum F;
	fresnel->Evaluate(tspack, CosTheta(wo), &F);
	return F.filter(tspack) / fabsf(CosTheta(wo));
}

bool ArchitecturalReflection::Sample_f(const TsPack *tspack, const Vector &wo,
	Vector *wi, float u1, float u2, SWCSpectrum *const f, float *pdf, float *pdfBack, bool reverse) const
{
	if (wo.z <= 0.f) {
		*pdf = 0.f;
		if (pdfBack)
			*pdfBack = 0.f;
		return false;
	}
	return SpecularReflection::Sample_f(tspack, wo, wi, u1, u2, f, pdf, pdfBack, reverse);
}
float ArchitecturalReflection::Weight(const TsPack *tspack, const Vector &wo, bool reverse) const
{
	if (wo.z <= 0.f)
		return 0.f;
	return SpecularReflection::Weight(tspack, wo, reverse);
}
