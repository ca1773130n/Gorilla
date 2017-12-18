/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   path.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - path.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// path.cpp*
#include "girl.h"
#include "transport.h"
#include "scene.h"

namespace girl
{

// PathIntegrator Declarations
class GIRLCOREDLL  PathIntegrator : public SurfaceIntegrator {
public:
	// PathIntegrator types
	enum LightStrategy { SAMPLE_ALL_UNIFORM, SAMPLE_ONE_UNIFORM,
		SAMPLE_AUTOMATIC
	};
	enum RRStrategy { RR_EFFICIENCY, RR_PROBABILITY, RR_NONE };

	// PathIntegrator Public Methods
	SWCSpectrum Li(const TsPack *tspack, const Scene *scene, const RayDifferential &ray, const Sample *sample, float *newAlpha) const;
	void RequestSamples(Sample *sample, const Scene *scene);
	PathIntegrator(LightStrategy st, RRStrategy rst, int md, float cp) {
		lightStrategy = st;
		rrStrategy = rst;
		maxDepth = md;
		continueProbability = cp;
		bufferId = -1;
	}
	void Preprocess(const TsPack *tspack, const Scene *scene);
	virtual ~PathIntegrator() { }
	static SurfaceIntegrator *CreateSurfaceIntegrator(const ParamSet &params);
private:
	// PathIntegrator Private Data
	LightStrategy lightStrategy;
	RRStrategy rrStrategy;
	int maxDepth;
	float continueProbability;
	// Declare sample parameters for light source sampling
	int sampleOffset, bufferId;
};

}//namespace girl

