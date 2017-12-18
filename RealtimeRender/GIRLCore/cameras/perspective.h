/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   perspective.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - perspective.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// perspective.cpp*
#include "camera.h"

namespace girl
{

// PerspectiveCamera Declarations
class PerspectiveCamera : public ProjectiveCamera {
public:
	// PerspectiveCamera Public Methods
	PerspectiveCamera(const Transform &world2cam,
		const float Screen[4], float hither, float yon,
		float sopen, float sclose,
		float lensr, float focald, bool autofocus, float fov,
		int distribution, int shape, int power,
		Film *film);
	float GenerateRay(const Sample &sample, Ray *) const;
	bool Sample_W(const TsPack *tspack, const Scene *scene, float u1, float u2, float u3, BSDF **bsdf, float *pdf, SWCSpectrum *We) const;
	bool Sample_W(const TsPack *tspack, const Scene *scene, const Point &p, const Normal &n, float u1, float u2, float u3, BSDF **bsdf, float *pdf, float *pdfDirect, VisibilityTester *visibility, SWCSpectrum *We) const;
	float Pdf(const Point &p, const Normal &n, const Vector &wi) const;
	void GetSamplePosition(const Point &p, const Vector &wi, float *x, float *y) const;
	bool IsDelta() const
	{
		return LensRadius==0.0f;
	}
	void AutoFocus(Scene* scene);
	void SampleLens(float u1, float u2, float *dx, float *dy) const;

	static Camera *CreateCamera(const Transform &world2cam, const ParamSet &params, Film *film);

private:
	Point pos;
	Normal normal;
	float fov;
	float posPdf;
	float screen[4];
	float R,xWidth,yHeight,xPixelWidth,yPixelHeight,Apixel;
	int distribution, shape, power;

	// Dade - field used for autofocus feature
	bool autoFocus;

	boost::shared_ptr<Shape> lens;
};

}//namespace girl
