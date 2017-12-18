/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   camera.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - camera.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_CAMERA_H
#define GIRL_CAMERA_H
// camera.h*
#include "girl.h"
#include "geometry/transform.h"
#include "error.h"

namespace girl
{
// Camera Declarations
class GIRLCOREDLL   Camera {
public:
	// Camera Interface
	Camera(const Transform &world2cam, float hither, float yon,
		float sopen, float sclose, Film *film);
	virtual ~Camera();
	virtual float GenerateRay(const Sample &sample, Ray *ray) const = 0;
	virtual bool Sample_W(const TsPack *tspack, const Scene *scene, float u1, float u2, float u3, BSDF **bsdf, float *pdf, SWCSpectrum *We) const {girlError(GIRL_BUG, GIRL_SEVERE, "Unimplemented Camera::Sample_W"); return false;}
	virtual bool Sample_W(const TsPack *tspack, const Scene *scene, const Point &p, const Normal &n, float u1, float u2, float u3, BSDF **bsdf, float *pdf, float *pdfDirect, VisibilityTester *visibility, SWCSpectrum *We) const {girlError(GIRL_BUG, GIRL_SEVERE, "Unimplemented Camera::Sample_W"); return false;}
	virtual void GetSamplePosition(const Point &p, const Vector &wi, float *x, float *y) const {}
	virtual bool IsDelta() const;
	virtual void AutoFocus(Scene* scene) { }

	// Camera Public Data
	Film *film;
protected:
	// Camera Protected Data
	Transform WorldToCamera, CameraToWorld;
	float ClipHither, ClipYon;
	float ShutterOpen, ShutterClose;
};
class GIRLCOREDLL   ProjectiveCamera : public Camera {
public:
	// ProjectiveCamera Public Methods
	ProjectiveCamera(const Transform &world2cam,
	    const Transform &proj, const float Screen[4],
		float hither, float yon,
		float sopen, float sclose,
		float lensr, float focald, Film *film);
protected:
	bool GenerateSample(const Point &p, Sample *sample) const;
	// ProjectiveCamera Protected Data
	Transform CameraToScreen, WorldToScreen, RasterToCamera;
	Transform ScreenToRaster, RasterToScreen;
	Transform WorldToRaster, RasterToWorld;
	float LensRadius, FocalDistance;
};

}//namespace girl

#endif // GIRL_CAMERA_H
