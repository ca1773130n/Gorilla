/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   sky.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - sky.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// sky.h*
#include "girl.h"
#include "light.h"
#include "texture.h"
#include "shape.h"
#include "scene.h"
#include "spd.h"

namespace girl
{

// SkyLight Declarations
class GIRLCOREDLL  SkyLight : public Light {
public:
	// SkyLight Public Methods
	SkyLight(const Transform &light2world,	const float skyscale, int ns, Vector sd, float turb, float aconst, float bconst, float cconst, float dconst, float econst);
	~SkyLight();
	SWCSpectrum Power(const TsPack *tspack, const Scene *scene) const {
		Point worldCenter;
		float worldRadius;
		scene->WorldBound().BoundingSphere(&worldCenter,
		                                    &worldRadius);
		//return Lbase * GetSkySpectralRadiance(.0, .0) * M_PI * worldRadius * worldRadius;
		return skyScale * M_PI * worldRadius * worldRadius;
	}
	bool IsDeltaLight() const { return false; }
	SWCSpectrum Le(const TsPack *tspack, const RayDifferential &r) const;
	SWCSpectrum Le(const TsPack *tspack, const Scene *scene, const Ray &r,
		const Normal &n, BSDF **bsdf, float *pdf, float *pdfDirect) const;
	SWCSpectrum Sample_L(const TsPack *tspack, const Point &p, const Normal &n,
		float u1, float u2, float u3, Vector *wi, float *pdf,
		VisibilityTester *visibility) const;
	SWCSpectrum Sample_L(const TsPack *tspack, const Point &p, float u1, float u2, float u3,
		Vector *wi, float *pdf, VisibilityTester *visibility) const;
	SWCSpectrum Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2,
		float u3, float u4, Ray *ray, float *pdf) const;
	float Pdf(const Point &, const Normal &, const Vector &) const;
	float Pdf(const Point &, const Vector &) const;
	bool Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2, float u3, BSDF **bsdf, float *pdf, SWCSpectrum *Le) const;
	bool Sample_L(const TsPack *tspack, const Scene *scene, const Point &p, const Normal &n, float u1, float u2, float u3, BSDF **bsdf, float *pdf, float *pdfDirect, VisibilityTester *visibility, SWCSpectrum *Le) const;

	static Light *CreateLight(const Transform &light2world,
		const ParamSet &paramSet);

		// internal methods
	Vector		GetSunPosition() const;
	void		SunThetaPhi(float &theta, float &phi) const;
	RGBColor	GetSunSpectralRadiance() const;
	float		GetSunSolidAngle() const;
	void		GetSkySpectralRadiance(const TsPack *tspack, const float theta, const float phi, SWCSpectrum * const dst_spect) const;
	void		GetAtmosphericEffects(const Vector &viewer, const Vector &source,
									RGBColor &atmAttenuation, RGBColor &atmInscatter ) const;

	void		InitSunThetaPhi();
	void		ChromaticityToSpectrum(const TsPack *tspack, const float x, const float y, SWCSpectrum * const dst_spect) const;
	float		PerezFunction(const float *lam, float theta, float phi, float lvz) const;

private:
	// SkyLight Private Data
	float skyScale;
	Vector  sundir;
	float 	turbidity;
	float	thetaS, phiS;
	float zenith_Y, zenith_x, zenith_y;
	float perez_Y[6], perez_x[6], perez_y[6];
	SPD *D65SPD;
};

}//namespace girl

