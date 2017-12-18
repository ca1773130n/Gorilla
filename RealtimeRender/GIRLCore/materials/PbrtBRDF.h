/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   PbrtBRDF.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - PbrtBRDF.h initial implementation
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
#if 0
#include "pbrt.h"
#include "material.h"
#include "PbrtBRDFRead.h"
#include "PbrtBxDF.h"
#endif

#include "girl.h"
#include "bxdf.h"
#include "material.h"
#include "PbrtBRDFRead.h"
#include "PbrtBxDF.h"

namespace girl {

// PbrtBRDF Class Declarations
class GIRLCOREDLL  PbrtBRDF : public Material {
public:
	// PbrtBRDF Public Methods
	PbrtBRDF(
			const string &material_type,
			const string &reflection_type,
			bool uniform_pdf,
//			boost::shared_ptr<Texture<RGBColor> > kd,
			boost::shared_ptr<Texture<SWCSpectrum> > kd,
			boost::shared_ptr<Texture<float> > sig,
			boost::shared_ptr<Texture<float> > bump);

//	BSDF *GetBSDF(const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading) const;
	BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;

	static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);

private:
	// PbrtBRDF Private Data
	
//	boost::shared_ptr<Texture<RGBColor> > Kd;
	boost::shared_ptr<Texture<SWCSpectrum> > Kd;
	boost::shared_ptr<Texture<float> > sigma, bumpMap;

	double* PbrtBRDF_table;
	double* PbrtPDF_table;

	BxDFType reflection_type;
	bool uniform_pdf;

	void TestBRDFLoad();
};

}; // end of namespace