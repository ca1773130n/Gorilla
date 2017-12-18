/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   distant.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - distant.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// distant.cpp*
#include "girl.h"
#include "light.h"
#include "shape.h"
#include "scene.h"

namespace girl
{

// DistantLight Declarations
class GIRLCOREDLL  DistantLight : public Light {
public:
	// DistantLight Public Methods
	DistantLight(const Transform &light2world, const RGBColor &radiance, float gain, const Vector &dir);
	~DistantLight();
	bool IsDeltaLight() const { return true; }
	SWCSpectrum Power(const TsPack *tspack, const Scene *scene) const {
		Point worldCenter;
		float worldRadius;
		scene->WorldBound().BoundingSphere(&worldCenter,
		                                   &worldRadius);
		return SWCSpectrum(tspack, LSPD) * M_PI * worldRadius * worldRadius;
	}
	SWCSpectrum Sample_L(const TsPack *tspack, const Point &P, float u1, float u2, float u3,
		Vector *wo, float *pdf, VisibilityTester *visibility) const;
	SWCSpectrum Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2,
		float u3, float u4, Ray *ray, float *pdf) const;
	float Pdf(const Point &, const Vector &) const;
	
	static Light *CreateLight(const Transform &light2world,
		const ParamSet &paramSet);
private:
	// DistantLight Private Data
	Vector lightDir;
	SPD *LSPD;
};

}//namespace girl
