/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   goniometric.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - goniometric.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// goniometric.cpp*
#include "girl.h"
#include "light.h"
#include "shape.h"
#include "scene.h"
#include "mipmap.h"

namespace girl
{

// GonioPhotometricLight Declarations
class GIRLCOREDLL  GonioPhotometricLight : public Light {
public:
	// GonioPhotometricLight Public Methods
	GonioPhotometricLight(const Transform &light2world, const RGBColor &, const
	string &texname);
	~GonioPhotometricLight() { delete mipmap; }
	bool IsDeltaLight() const { return true; }
	RGBColor Scale(const Vector &w) const {
		Vector wp = Normalize(WorldToLight(w));
		swap(wp.y, wp.z);
		float theta = SphericalTheta(wp);
		float phi   = SphericalPhi(wp);
		float s = phi * INV_TWOPI, t = theta * INV_PI;
		return mipmap ? mipmap->Lookup(t, s) : 1.f;
	}
	SWCSpectrum Power(const TsPack *tspack, const Scene *) const {
		return SWCSpectrum(tspack, 4.f * M_PI * Intensity *
			mipmap->Lookup(.5f, .5f, .5f));
	}
	SWCSpectrum Sample_L(const TsPack *tspack, const Point &P, float u1, float u2, float u3,
		Vector *wo, float *pdf, VisibilityTester *visibility) const;
	SWCSpectrum Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2,
			float u3, float u4, Ray *ray, float *pdf) const;
	float Pdf(const Point &, const Vector &) const;
	
	static Light *CreateLight(const Transform &light2world,
		const ParamSet &paramSet);
private:
	// GonioPhotometricLight Private Data
	Point lightPos;
	RGBColor Intensity;
	MIPMap<RGBColor> *mipmap;

};

}//namespace girl

