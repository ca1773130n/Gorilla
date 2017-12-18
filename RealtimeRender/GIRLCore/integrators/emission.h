/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   emission.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - emission.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// emission.cpp*
#include "volume.h"
#include "transport.h"
#include "scene.h"

namespace girl
{

// EmissionIntegrator Declarations
class GIRLCOREDLL  EmissionIntegrator : public VolumeIntegrator {
public:
	// EmissionIntegrator Public Methods
	EmissionIntegrator(float ss) { stepSize = ss; }
	void RequestSamples(Sample *sample, const Scene *scene);
	void Transmittance(const TsPack *tspack, const Scene *, const Ray &ray,
		const Sample *sample, float *alpha, SWCSpectrum *const L) const;
	SWCSpectrum Li(const TsPack *tspack, const Scene *, const RayDifferential &ray, const Sample *sample, float *alpha) const;
	static VolumeIntegrator *CreateVolumeIntegrator(const ParamSet &params);
private:
	// EmissionIntegrator Private Data
	float stepSize;
	int tauSampleOffset, scatterSampleOffset;
};

}//namespace girl

