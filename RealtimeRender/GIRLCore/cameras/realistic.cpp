/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   realistic.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - realistic.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

/*
* Realistic camera, based on:
* Realistic Camera Component plugin Copyright(c) 2004 Nico Galoppo von Borries
*/

// realistic.ccp
#include "realistic.h"
#include "sampling.h"
#include "shape.h"
#include "mc.h"
#include "film.h"
#include "dynload.h"
#include "paramset.h"
#include "dynload.h"

#include <fstream>
using std::ifstream;

using namespace girl;

RealisticCamera::RealisticCamera(const Transform &world2cam,
                 const float Screen[4],
				 float hither, float yon, 
				 float sopen, float sclose, 
				 float filmdistance, float aperture_diameter, string specfile, 
				 float filmdiag, Film *f)
	: Camera(world2cam, hither, yon, sopen, sclose, f) 
{
    filmDistance = filmdistance;
    filmDist2 = filmDistance * filmDistance;
    apertureDiameter = aperture_diameter;
    filmDiag = filmdiag;
  
    distToBack = ParseLensData(specfile);

    // raster to camera and film to camera transforms
    float diag2 = sqrt(float(film->xResolution * film->xResolution +
                             film->yResolution * film->yResolution));
    float scale = diag2 / filmDiag;
    float w = film->xResolution / scale;
    float h = film->yResolution / scale;
    Transform FilmToRaster = 
        Scale(-scale, scale, 1.f) *
        Translate(Vector(-w/2.0f, h/2.0f, 0.f));
    RasterToFilm = FilmToRaster.GetInverse();
    FilmToCamera = Translate(Vector(0.f, 0.f, -filmDistance - distToBack));
    RasterToCamera =  FilmToCamera * RasterToFilm;
}   
RealisticCamera::~RealisticCamera(void) {
}
float RealisticCamera::GenerateRay(const Sample &sample, Ray *ray) const {
    // Generate raster and back lens samples
    Point Pras(sample.imageX, sample.imageY, 0.f);
    Point PCamera;
    RasterToCamera(Pras, &PCamera);
    float lensU, lensV;
    ConcentricSampleDisk(sample.lensU, sample.lensV, &lensU, &lensV);
    lensU *= backAperture;
    lensV *= backAperture;
    Point PBack(lensU, lensV, -distToBack);

    ray->o = PCamera;
    ray->d = Normalize(PBack - PCamera);
    ray->time = Lerp(sample.time, ShutterOpen, ShutterClose);
    ray->mint = 0.;
    ray->maxt = INFINITY;

    float cos4 = ray->d.z;
    cos4 *= cos4;
    cos4 *= cos4;

    // Iterate over the lens components, and intersect
    DifferentialGeometry dg;
    float thit;
    for (int i = (int)lenses.size() -1 ; i >= 0; --i) {
        if (lenses[i]->shape->Intersect(*ray, &thit, &dg)) {
            // intersection, compute refracted ray
            Normal n = (lenses[i]->entering == true) ? dg.nn : -dg.nn;
            float eta = lenses[i]->eta;
            float cos_i = Dot(-ray->d, n);
            float sint2 = (eta * eta * (1 - cos_i*cos_i));
            if (sint2 > 1.) { // total internal reflection, return dead ray
                ray->mint = 1.f;
                ray->maxt = 0.f;
                return 1.f;
            }
            // use snell's law
            float cost = sqrtf(max(0.f, 1.f - sint2));
            float nscale = eta * cos_i - cost;
            Vector d(n.x * nscale + eta * ray->d.x, 
                     n.y * nscale + eta * ray->d.y, 
                     n.z * nscale + eta * ray->d.z);

            ray->o = (*ray)(thit);
            ray->d = Normalize(d);
            ray->mint = 0.f;
            ray->maxt = INFINITY;
        }
        else { // no intersection, return dead ray
            ray->mint = 1.f;
            ray->maxt = 0.f;
            return 1.f;
        }
    }
    ray->maxt = (ClipYon - ClipHither) / ray->d.z;
    CameraToWorld(*ray, ray);
    return cos4 / filmDist2;
}

float RealisticCamera::ParseLensData(const string& specfile) {
    // parse lens description
    ifstream file;
    file.open(specfile.c_str());
    if (!file)
        printf("Couldn't open camera specfile...");
    string lineread;
    float r, sep, nt, aperture, ni = 1.f;
    float accumdist = 0.;
    bool entering;
    lenses.clear();
    
    while (std::getline(file, lineread))
    {
        if (sscanf(lineread.c_str(), "%f\t%f\t%f\t%f\n", 
            &r, &sep, &nt, &aperture) == 4)
        {
            // stop is a disc instead of a sphere
            if (r == 0.0) {
                ParamSet paramSet;
                float height = -accumdist;
                float radius = apertureDiameter / 2.0f;
                float innerradius = 0.f;
                paramSet.AddFloat("height", &height);
                paramSet.AddFloat("radius", &radius);
                paramSet.AddFloat("innerradius", &innerradius);
                boost::shared_ptr<Shape> shape = 
                    MakeShape("disk", Transform(), false, paramSet);
				boost::shared_ptr<Lens> o (new Lens(false, 1.f, aperture, shape));
                lenses.push_back(o);
                ni = 1.f;
            }
            else {
                // camera to lens object transform
                float radius = fabsf(r);
                Transform lensToCam;
                if (r > 0) {
                    lensToCam = Translate(Vector(0., 0., -accumdist - radius));
                    entering = false;
                }
                else {
                    lensToCam = Translate(Vector(0., 0., -accumdist + radius));
                    entering = true;
                }
                ParamSet paramSet;
                paramSet.AddFloat("radius", &radius);
                paramSet.AddFloat("aperture", &aperture);
                boost::shared_ptr<Shape> shape = 
                    MakeShape("lenscomponent", lensToCam, false, 
                    paramSet);
				boost::shared_ptr<Lens> o (new Lens(entering, nt/ni, aperture, shape));
                lenses.push_back(o);
                ni = nt;
            }
            accumdist += sep;
        }
    }
    backAperture = aperture;
    return accumdist;
}

Camera* RealisticCamera::CreateCamera(const Transform &world2cam, const ParamSet &params,
	Film *film)
{
	// Extract common camera parameters from \use{ParamSet}
	float hither = params.FindOneFloat("hither", 1e-3f);
	float yon = params.FindOneFloat("yon", 1e30f);
	float shutteropen = params.FindOneFloat("shutteropen", 0.f);
	float shutterclose = params.FindOneFloat("shutterclose", 1.f);

	// Realistic camera-specific parameters
	string specfile = params.FindOneString("specfile", "");
	float filmdistance = params.FindOneFloat("filmdistance", 70.0); // about 70 mm default to film
 	float fstop = params.FindOneFloat("aperture_diameter", 1.0);	
	float filmdiag = params.FindOneFloat("filmdiag", 35.0);

	if (specfile == "") {
	    printf( "No lens spec file supplied!\n" );
	}

    float frame = float(film->xResolution)/float(film->yResolution);
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
	return new RealisticCamera(world2cam, screen, hither, yon,
				   shutteropen, shutterclose, filmdistance, fstop, 
				   specfile, filmdiag, film);
}

static DynamicLoader::RegisterCamera<RealisticCamera> r("realistic");
