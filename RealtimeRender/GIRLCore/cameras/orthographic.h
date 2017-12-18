/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   orthographic.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - orthographic.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
// orthographic.cpp*
#include "camera.h"

namespace girl
{

// OrthographicCamera Declarations
class GIRLCOREDLL  OrthoCamera : public ProjectiveCamera {
public:
	// OrthoCamera Public Methods
	OrthoCamera(const Transform &world2cam,
	            const float Screen[4],
		        float hither, float yon,
				float sopen, float sclose,
				float lensr, float focald, bool autofocus, Film *film);
	float GenerateRay(const Sample &sample, Ray *) const;
	bool IsVisibleFromEyes(const Scene *scene, const Point &lenP, const Point &worldP, Sample* sample_gen, Ray *ray_gen) const;
	float GetConnectingFactor(const Point &lenP, const Point &worldP, const Vector &wo, const Normal &n) const;
	void GetFgirl2RadianceFactors(Film *film, float *factors, int xPixelCount, int yPixelCount) const;
	bool IsDelta() const
	{
		return true;
	}
	void SamplePosition(float u1, float u2, float u3, Point *p, float *pdf) const;
	float EvalPositionPdf() const;
	void AutoFocus(Scene* scene);

	//float SampleDirection(const Sample &sample, Ray *ray)
	//{
	//	Point Pras(sample.imageX, sample.imageY, 0);
	//	RasterToCamera(Pras, &(ray->o));
	//	ray->d = Vector(0,0,1);
	//	ray->mint = 0.;
	//	ray->maxt = ClipYon - ClipHither;
	//	CameraToWorld(*ray, ray);
	//	return 1.0f;
	//}
	//float EvalDirectionPdf(Film *film, const Vector& wo, const Sample &sample, const Point& p)
	//{
	//	return 1.0f;
	//}
	//SWCSpectrum EvalValue()
	//{
	//	return SWCSpectrum(1.0f);
	//}

	static Camera *CreateCamera(const Transform &world2cam, const ParamSet &params, Film *film);

private:	
	// Dade - field used for autofocus feature
	bool autoFocus;

	float screenDx,screenDy;
};

}//namespace girl
