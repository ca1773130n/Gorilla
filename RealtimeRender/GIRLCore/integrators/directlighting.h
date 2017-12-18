/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   directlighting.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - directlighting.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// directlighting.cpp*
#include "girl.h"
#include "light.h"
#include "transport.h"
#include "scene.h"

namespace girl
{

// DirectLightingIntegrator Declarations
class GIRLCOREDLL  DirectLightingIntegrator : public SurfaceIntegrator {
public:
	// DirectLightingIntegrator types
	enum LightStrategy { 
		SAMPLE_ALL_UNIFORM, 
		SAMPLE_ONE_UNIFORM,
		SAMPLE_AUTOMATIC
	};

	// DirectLightingIntegrator Public Methods
	DirectLightingIntegrator(LightStrategy ls, int md);
	~DirectLightingIntegrator() { }

	SWCSpectrum Li(const TsPack *tspack, const Scene *scene,
		const RayDifferential &ray, const Sample *sample, float *alpha) const;
	void RequestSamples(Sample *sample, const Scene *scene);

	static SurfaceIntegrator *CreateSurfaceIntegrator(const ParamSet &params);

private:
	SWCSpectrum LiInternal(const TsPack *tspack, const Scene *scene, const RayDifferential &ray,
		const Sample *sample, float *alpha, int rayDepth) const;

	// DirectLighting Private Data
	LightStrategy lightStrategy;

	int maxDepth; // NOBOOK
	// Declare sample parameters for light source sampling
	int sampleOffset;
};

}//namespace girl
