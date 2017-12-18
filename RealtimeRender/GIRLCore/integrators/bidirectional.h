/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   bidirectional.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - bidirectional.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// bidirectional.cpp*
#include "girl.h"
#include "transport.h"

namespace girl
{

// Bidirectional Local Declarations
class GIRLCOREDLL  BidirIntegrator : public SurfaceIntegrator {
public:
	// PathIntegrator types
	enum LightStrategy { SAMPLE_ALL_UNIFORM, SAMPLE_ONE_UNIFORM,
		SAMPLE_AUTOMATIC
	};
//	enum RRStrategy { RR_EFFICIENCY, RR_PROBABILITY, RR_NONE };

	BidirIntegrator(int ed, int ld, LightStrategy ls) : lightStrategy(ls), maxEyeDepth(ed), maxLightDepth(ld) {
		eyeBufferId = -1;
		lightBufferId = -1;
	}
	// BidirIntegrator Public Methods
	SWCSpectrum Li(const TsPack *tspack, const Scene *scene, const RayDifferential &ray, const Sample *sample, float *alpha) const;
	void RequestSamples(Sample *sample, const Scene *scene);
	void Preprocess(const TsPack *tspack, const Scene *scene);
	static SurfaceIntegrator *CreateSurfaceIntegrator(const ParamSet &params);
private:
	// BidirIntegrator Data
	LightStrategy lightStrategy;
	int maxEyeDepth, maxLightDepth;
	int lightNumOffset, lightComponentOffset, lightPosOffset, lightDirOffset;
	int sampleEyeOffset, sampleLightOffset, sampleDirectOffset;
	int eyeBufferId, lightBufferId;
};

}//namespace girl

