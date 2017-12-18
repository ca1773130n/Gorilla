/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   orthographic.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - orthographic.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// orthographic.cpp*
#include "orthographic.h"
#include "sampling.h"
#include "mc.h"
#include "scene.h" // for struct Intersection
#include "film.h" // for Film
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// OrthographicCamera Definitions
OrthoCamera::OrthoCamera(const Transform &world2cam,
		const float Screen[4], float hither, float yon,
		float sopen, float sclose, float lensr,
		float focald, bool autofocus, Film *f)
	: ProjectiveCamera(world2cam, Orthographic(hither, yon),
		 Screen, hither, yon, sopen, sclose,
		 lensr, focald, f), autoFocus(autofocus) {
	 screenDx = Screen[1] - Screen[0];
	 screenDy = Screen[3] - Screen[2];//FixMe: 3-2 or 2-3
}

void OrthoCamera::AutoFocus(Scene* scene)
{
	if (autoFocus) {
		std::stringstream ss;

		// Dade - trace a ray in the middle of the screen
		
		int xstart, xend, ystart, yend;
		film->GetSampleExtent(&xstart, &xend, &ystart, &yend);
		Point Pras((xend - xstart) / 2, (yend - ystart) / 2, 0);

		// Dade - debug code
		//ss.str("");
		//ss << "Raster point: " << Pras;
		//girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

		Point Pcamera;
		RasterToCamera(Pras, &Pcamera);
		Ray ray;
		ray.o = Pcamera;
		ray.d = Vector(0,0,1);

		// Dade - I wonder what time I could use here
		ray.time = 0.0f;
		
		ray.mint = 0.f;
		ray.maxt = ClipYon - ClipHither;
		CameraToWorld(ray, &ray);

		// Dade - debug code
		//ss.str("");
		//ss << "Ray.o: " << ray.o << " Ray.d: " << ray.d;
		//girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

		Intersection isect;
		if (scene->Intersect(ray, &isect))
			FocalDistance = ray.maxt;
		else
			girlError(GIRL_NOERROR, GIRL_WARNING, "Unable to define the Autofocus focal distance");

		ss.str("");
		ss << "Autofocus focal distance: " << FocalDistance;
		girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());
	}
}

float OrthoCamera::GenerateRay(const Sample &sample, Ray *ray) const
{
	// Generate raster and camera samples
	Point Pras(sample.imageX, sample.imageY, 0);
	Point Pcamera;
	RasterToCamera(Pras, &Pcamera);
	ray->o = Pcamera;
	ray->d = Vector(0,0,1);
	// Set ray time value
	ray->time = Lerp(sample.time, ShutterOpen, ShutterClose);

	// Modify ray for depth of field
	if (LensRadius > 0.) {
		// Sample point on lens
		float lensU, lensV;
		ConcentricSampleDisk(sample.lensU, sample.lensV,
		                     &lensU, &lensV);
		lensU *= LensRadius;
		lensV *= LensRadius;
		// Compute point on plane of focus
		float ft = (FocalDistance - ClipHither) / ray->d.z;
		Point Pfocus = (*ray)(ft);
		// Update ray for effect of lens
		ray->o.x += lensU;
		ray->o.y += lensV;
		ray->d = Pfocus - ray->o;
	}

	ray->mint = 0.;
	ray->maxt = ClipYon - ClipHither;
	ray->d = Normalize(ray->d);
	CameraToWorld(*ray, ray);
	return 1.f;
}

bool OrthoCamera::IsVisibleFromEyes(const Scene *scene, const Point &lenP, const Point &worldP, Sample* sample_gen, Ray *ray_gen) const
{
	bool isVisible = false;
	if (GenerateSample(worldP, sample_gen))
	{
		GenerateRay(*sample_gen, ray_gen);
		if (WorldToCamera(worldP).z>0)
		{
			ray_gen->maxt = Distance(ray_gen->o, worldP)*(1-RAY_EPSILON);
			isVisible = !scene->IntersectP(*ray_gen);
		}
	}
	return isVisible;
}

float OrthoCamera::GetConnectingFactor(const Point &lenP, const Point &worldP, const Vector &wo, const Normal &n) const
{
	return AbsDot(wo, n);
}

void OrthoCamera::GetFgirl2RadianceFactors(Film *film, float *factors, int xPixelCount, int yPixelCount) const
{
	float Apixel = (screenDx*screenDy/(film->xResolution*film->yResolution));
	int x,y;
	float invApixel = 1/Apixel;
	for (y = 0; y < yPixelCount; ++y) {
		for (x = 0; x < xPixelCount; ++x) {
			factors[x+y*xPixelCount] =  invApixel;
		}
	}
}

void OrthoCamera::SamplePosition(float u1, float u2, float u3, Point *p, float *pdf) const
{
	// orthographic camera is composed of many parallel pinhole cameras with little fov.
	*pdf = 1.0f;
}

float OrthoCamera::EvalPositionPdf() const
{
	return 1.0f/(screenDx*screenDy);
}

Camera* OrthoCamera::CreateCamera(const Transform &world2cam, const ParamSet &params,
	Film *film)
{
	// Extract common camera parameters from _ParamSet_
	float hither = max(1e-4f, params.FindOneFloat("hither", 1e-3f));
	float yon = min(params.FindOneFloat("yon", 1e30f), 1e30f);
	float shutteropen = params.FindOneFloat("shutteropen", 0.f);
	float shutterclose = params.FindOneFloat("shutterclose", 1.f);
	float lensradius = params.FindOneFloat("lensradius", 0.f);
	float focaldistance = params.FindOneFloat("focaldistance", 1e30f);
	bool autofocus = params.FindOneBool("autofocus", false);
	float frame = params.FindOneFloat("frameaspectratio",
		float(film->xResolution)/float(film->yResolution));
	float screen[4];
	if (frame > 1.f) {
		screen[0] = -frame;
		screen[1] =  frame;
		screen[2] = -1.f;
		screen[3] =  1.f;
	}
	else {
		screen[0] = -1.f;
		screen[1] =  1.f;
		screen[2] = -1.f / frame;
		screen[3] =  1.f / frame;
	}
	int swi;
	const float *sw = params.FindFloat("screenwindow", &swi);
	if (sw && swi == 4)
		memcpy(screen, sw, 4*sizeof(float));
	return new OrthoCamera(world2cam, screen, hither, yon,
		shutteropen, shutterclose, lensradius, focaldistance, autofocus,
		film);
}

static DynamicLoader::RegisterCamera<OrthoCamera> r("orthographic");
