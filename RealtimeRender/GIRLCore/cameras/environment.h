/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   environment.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - environment.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// environment.cpp*
#include "camera.h"

namespace girl
{

// EnvironmentCamera Declarations
class GIRLCOREDLL  EnvironmentCamera : public Camera {
public:
	// EnvironmentCamera Public Methods
	EnvironmentCamera(const Transform &world2cam, float hither,
		float yon, float sopen, float sclose, Film *film);
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
	//float SampleDirection(const Sample &sample, Ray *ray)
	//{
	//	// FixMe: Duplicated code
	//	ray->o = rayOrigin;
	//	// Generate environment camera ray direction
	//	float theta = M_PI * sample.imageY / film->yResolution;
	//	float phi = 2 * M_PI * sample.imageX / film->xResolution;
	//	Vector dir(sinf(theta) * cosf(phi), cosf(theta),
	//		sinf(theta) * sinf(phi));
	//	CameraToWorld(dir, &ray->d);
	//	// Set ray time value
	//	ray->time = Lerp(sample.time, ShutterOpen, ShutterClose);
	//	ray->mint = ClipHither;
	//	ray->maxt = ClipYon;

	//	// R*R/(Apixel*nx*ny)
	//	// sub-pdf in Apixel is not correct
	//	return 1.0f/(2.0f*M_PI*M_PI*sinf(M_PI*((int)sample.imageY+0.5f)/film->yResolution));

	//}
	//float EvalDirectionPdf(Film *film, const Vector& wo, const Sample &sample, const Point& p)
	//{
	//	return 1.0f/(2.0f*M_PI*M_PI*sinf(M_PI*((int)sample.imageY+0.5f)/film->yResolution));
	//}
	//SWCSpectrum EvalValue()
	//{
	//	return SWCSpectrum(1.0f);
	//}
	static Camera *CreateCamera(const Transform &world2cam, const ParamSet &params, Film *film);
private:
	bool GenerateSample(const Point &p, Sample *sample) const;
	// EnvironmentCamera Private Data
	Point rayOrigin;
};

}//namespace girl
