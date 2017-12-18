/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   distributedpath.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - distributedpath.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// distributedpath.cpp*
#include "girl.h"
#include "light.h"
#include "transport.h"
#include "scene.h"

namespace girl
{

// DistributedPath Declarations

class GIRLCOREDLL  DistributedPath : public SurfaceIntegrator {
public:
	// DistributedPath types
	enum LightStrategy { SAMPLE_ALL_UNIFORM, SAMPLE_ONE_UNIFORM,
		SAMPLE_AUTOMATIC
	};

	// DistributedPath Public Methods
	DistributedPath(LightStrategy st, bool da, int ds, bool dd, bool dg, bool ida, int ids, bool idd, bool idg,
								 int drd, int drs, int dtd, int dts, int grd, int grs, int gtd, int gts, int srd, int std);
	~DistributedPath() { }

	SWCSpectrum Li(const TsPack *tspack, const Scene *scene, const RayDifferential &ray, const Sample *sample,
		float *alpha) const;
	void RequestSamples(Sample *sample, const Scene *scene);
	void Preprocess(const TsPack *tspack, const Scene *scene);

	static SurfaceIntegrator *CreateSurfaceIntegrator(const ParamSet &params);

private:
	SWCSpectrum LiInternal(const TsPack *tspack, const Scene *scene, const RayDifferential &ray,
		const Sample *sample, float *alpha, int rayDepth, bool includeEmit, float &nrContribs) const;

	// DistributedPath Private Data
	LightStrategy lightStrategy;
	bool directAll, directDiffuse, directGlossy, 
		indirectAll, indirectDiffuse, indirectGlossy;
	int directSamples, indirectSamples;
	int diffusereflectDepth, diffusereflectSamples, diffuserefractDepth, diffuserefractSamples, glossyreflectDepth, glossyreflectSamples,
		glossyrefractDepth, glossyrefractSamples, specularreflectDepth, specularrefractDepth, maxDepth;

	// Declare sample parameters for light source sampling
	int sampleOffset, bufferId;
	int lightSampleOffset, lightNumOffset, bsdfSampleOffset, bsdfComponentOffset;
	int indirectlightSampleOffset, indirectlightNumOffset, indirectbsdfSampleOffset, indirectbsdfComponentOffset;
	int diffuse_reflectSampleOffset, diffuse_reflectComponentOffset, indirectdiffuse_reflectSampleOffset, indirectdiffuse_reflectComponentOffset;
	int diffuse_refractSampleOffset, diffuse_refractComponentOffset, indirectdiffuse_refractSampleOffset, indirectdiffuse_refractComponentOffset;
	int glossy_reflectSampleOffset, glossy_reflectComponentOffset, indirectglossy_reflectSampleOffset, indirectglossy_reflectComponentOffset;
	int glossy_refractSampleOffset, glossy_refractComponentOffset, indirectglossy_refractSampleOffset, indirectglossy_refractComponentOffset;
};

}//namespace girl
