/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   single.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - single.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// single.cpp*
#include "volume.h"
#include "transport.h"
#include "scene.h"

namespace girl
{

// SingleScattering Declarations
class GIRLCOREDLL  SingleScattering : public VolumeIntegrator {
public:
	// SingleScattering Public Methods
	SingleScattering(float ss) : stepSize(ss) { }

	void Transmittance(const TsPack *tspack, const Scene *, const Ray &ray,
		const Sample *sample, float *alpha, SWCSpectrum *const L) const;
	void RequestSamples(Sample *sample, const Scene *scene);
	SWCSpectrum Li(const TsPack *tspack, const Scene *, const RayDifferential &ray, const Sample *sample, float *alpha) const;

	static VolumeIntegrator *CreateVolumeIntegrator(const ParamSet &params);

private:
	// SingleScattering Private Data
	float stepSize;

	int tauSampleOffset, scatterSampleOffset;
};

}//namespace girl
