/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   MerlBRDFFit.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - MerlBRDFFit.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////


// MerlBRDFFit.h*
#if 0
#include "pbrt.h"
#include "material.h"
#include "BRDF_Vol_Pub.h"
#include "MerlBxDFFit.h"
#endif

#include "girl.h"
#include "material.h"
#include "BRDF_Vol_Pub.h"
#include "MerlBxDFFit.h"
#include <map>

using namespace std;

namespace girl {

#define SIZE_BUF 256

// to prevent redundant loading
// Note: the following two lines was not be added inside the class
//
static map<string, VsNurbVolume	*> dataMap;
map<string, VsNurbVolume *>::iterator dataMap_iterator;

// MerlBRDFFit Class Declarations
class GIRLCOREDLL  MerlBRDFFit : public Material {
public:
	// MerlBRDFFit Public Methods
	MerlBRDFFit(const string &type,
		const string &refl_type,
		boost::shared_ptr<Texture<SWCSpectrum> > kd,
		boost::shared_ptr<Texture<float> > sig,
		boost::shared_ptr<Texture<float> > bump,
		boost::shared_ptr<Texture<float> > r,
		boost::shared_ptr<Texture<float> > g,
		boost::shared_ptr<Texture<float> > b		
		);

//	BSDF *GetBSDF(const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading) const;
	BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;

	static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);

private:
	// MerlBRDFFit Private Data
	boost::shared_ptr<Texture<SWCSpectrum> > Kd;
	boost::shared_ptr<Texture<float> > sigma, bumpMap;

	VsNurbVolume	*MerlBRDF_volume;
	BxDFType reflection_type;

	// RGB control factors.
	boost::shared_ptr<Texture<float> > fR, fG, fB;

	void TestBRDFLoad();
};
}; // end of namespace