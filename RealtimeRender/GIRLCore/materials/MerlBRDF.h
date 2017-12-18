/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   MerlBRDF.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - MerlBRDF.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "girl.h"
#include "material.h"
#include "MerlBRDFRead.h"
#include "MerlBxDF.h"
#include <map>

using namespace std;

namespace girl {

#define SIZE_BUF 256

// to prevent redundant loading
// Note: the following two lines was not be added inside the class
//
static map<string, double *> dataMap;
map<string, double *>::iterator dataMap_iterator;

// MerlBRDF Class Declarations
class GIRLCOREDLL  MerlBRDF : public Material {
public:
	// MerlBRDF Public Methods
	MerlBRDF(
			const string &material_type,
			const string &reflection_type,
			boost::shared_ptr<Texture<SWCSpectrum> > kd,
			boost::shared_ptr<Texture<float> > sig,
			boost::shared_ptr<Texture<float> > bump,
			boost::shared_ptr<Texture<float> > r,
			boost::shared_ptr<Texture<float> > g,
			boost::shared_ptr<Texture<float> > b
			);

	BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;

	static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);

private:
	// MerlBRDF Private Data
	
	boost::shared_ptr<Texture<SWCSpectrum> > Kd;
	boost::shared_ptr<Texture<float> > sigma, bumpMap;

	double* MerlBRDF_table;
	BxDFType reflection_type;

	// RGB control factors.
	boost::shared_ptr<Texture<float> > fR, fG, fB;
	
	void TestBRDFLoad();
	
};
};