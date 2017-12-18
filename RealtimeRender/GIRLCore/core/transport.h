/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   transport.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - transport.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_TRANSPORT_H
#define GIRL_TRANSPORT_H
// transport.h*
#include "girl.h"
#include "spectrum.h"

namespace girl
{

// Integrator Declarations
class GIRLCOREDLL   Integrator {
public:
	// Integrator Interface
	virtual ~Integrator();
	virtual SWCSpectrum Li(const TsPack *tspack, const Scene *scene,
					    const RayDifferential &ray,
					    const Sample *sample,
					    float *alpha) const = 0;
	virtual void Preprocess(const TsPack *tspack, const Scene *scene) {
	}
	virtual void RequestSamples(Sample *sample,
	                            const Scene *scene) {
	}
};

class GIRLCOREDLL  SurfaceIntegrator : public Integrator {
public:
	// Dade - Exphotonmap is the only integrator not supporting SWC at the moment
	virtual bool IsSWCSupported() {
		return true;
	}
};

class GIRLCOREDLL  VolumeIntegrator : public Integrator {
public:
	// modulates the supplied SWCSpectrum with the transmittance along the ray
	virtual void Transmittance(const TsPack *tspack, const Scene *scene,
		const Ray &ray, const Sample *sample, float *alpha, SWCSpectrum *const L) const = 0;
};

 SWCSpectrum EstimateDirect(const TsPack *tspack, const Scene *scene, const Light *light,
	const Point &p, const Normal &n, const Vector &wo, BSDF *bsdf, 
	const Sample *sample, 
	float &ls1, float &ls2, float &ls3, float &bs1, float &bs2, float &bcs);
 SWCSpectrum UniformSampleAllLights(const TsPack *tspack, const Scene *scene, const Point &p,
	const Normal &n, const Vector &wo, BSDF *bsdf,
	const Sample *sample, float *lightSample = NULL,
	float *lightNum = NULL, float *bsdfSample = NULL,
	float *bsdfComponent = NULL);
 SWCSpectrum UniformSampleOneLight(const TsPack *tspack, const Scene *scene, const Point &p,
	const Normal &n, const Vector &wo, BSDF *bsdf,
	const Sample *sample, float *lightSample = NULL,
	float *lightNum = NULL, float *bsdfSample = NULL,
	float *bsdfComponent = NULL);

// Note - Radiance - disabled as this code is broken. (not threadsafe)
/*
 SWCSpectrum WeightedSampleOneLight(const TsPack *tspack, const Scene *scene, const Point &p,
	const Normal &n, const Vector &wo, BSDF *bsdf,
	const Sample *sample, int lightSampleOffset, int lightNumOffset,
	int bsdfSampleOffset, int bsdfComponentOffset, float *&avgY,
	float *&avgYsample, float *&cdf, float &overallAvgY);
*/

}//namespace girl
 
#endif // GIRL_TRANSPORT_H
