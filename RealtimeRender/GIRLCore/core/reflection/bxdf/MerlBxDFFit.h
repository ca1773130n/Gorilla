/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   MerlBxDFFit.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - MerlBxDFFit.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#ifndef PBRT_MERLBXDFFIT_H
#define PBRT_MERLBXDFFIT_H
// MerlBxDF.h*

#if 0
#include "pbrt.h"
#include "reflection.h"
#include "BRDF_Vol_Pub.h"
#endif 

#include "girl.h"
#include "bxdf.h"
#include "color.h"
#include "spectrum.h"
#include "BRDF_Vol_Pub.h"

namespace girl
{
//
// Fitted Merl BxDF
//

class GIRLCOREDLL  MerlBxDFFit : public BxDF {
public:
	// MerlBxDFDebug Public Methods
	~MerlBxDFFit() {}

	MerlBxDFFit(VsNurbVolume* volume)
		: BxDF(BxDFType(BSDF_ALL_REFLECTION)),
		  MerlBRDF_volume(volume)
	{
		fR = 1.0;
		fG = 1.0;
		fB = 1.0;
	}

	MerlBxDFFit(VsNurbVolume* table, BxDFType type)
		: BxDF(type), MerlBRDF_volume(table)
	{
		fR = 1.0;
		fG = 1.0;
		fB = 1.0;
	} 

	MerlBxDFFit(VsNurbVolume* table, BxDFType type, float _r, float _g, float _b)
		: BxDF(type), MerlBRDF_volume(table), fR(_r), fG(_g), fB(_b)
	{
	} 

	void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const;

private:
	VsNurbVolume	*MerlBRDF_volume;

	// RGB control factors.
	float fR, fG, fB;
};
}; // end of namespace
#endif // PBRT_MERLBXDFFIT_H