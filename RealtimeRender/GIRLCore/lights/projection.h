/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   projection.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - projection.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// projection.cpp*
#include "girl.h"
#include "light.h"
#include "shape.h"
#include "mipmap.h"

namespace girl
{

// ProjectionLight Declarations
class GIRLCOREDLL  ProjectionLight : public Light {
public:
	// ProjectionLight Public Methods
	ProjectionLight(const Transform &light2world, const RGBColor &intensity,
		const string &texname, float fov);
	~ProjectionLight();
	bool IsDeltaLight() const { return true; }
	RGBColor Projection(const Vector &w) const;
	SWCSpectrum Power(const TsPack *tspack, const Scene *) const {
		return SWCSpectrum(tspack, Intensity * 2.f * M_PI * (1.f - cosTotalWidth) *
			projectionMap->Lookup(.5f, .5f, .5f));
	}
	SWCSpectrum Sample_L(const TsPack *tspack, const Point &P, float u1, float u2, float u3,
		Vector *wo, float *pdf, VisibilityTester *visibility) const;
	SWCSpectrum Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2,
			float u3, float u4, Ray *ray, float *pdf) const;
	float Pdf(const Point &, const Vector &) const;
	
	static Light *CreateLight(const Transform &light2world,
		const ParamSet &paramSet);
private:
	// ProjectionLight Private Data
	MIPMap<RGBColor> *projectionMap;
	Point lightPos;
	RGBColor Intensity;
	Transform lightProjection;
	float hither, yon;
	float screenX0, screenX1, screenY0, screenY1;
	float cosTotalWidth;
};

}//namespace girl
