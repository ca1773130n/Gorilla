/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   sampling.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - sampling.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// sampling.cpp*
#include "girl.h"
#include "sampling.h"
#include "scene.h"
#include "transport.h"
#include "volume.h"
#include "film.h"

using namespace girl;

// Sampler Method Definitions
Sampler::Sampler(int xstart, int xend, int ystart, int yend,
		int spp) {
	xPixelStart = xstart;
	xPixelEnd = xend;
	yPixelStart = ystart;
	yPixelEnd = yend;
	samplesPerPixel = spp;
	isSampleEnd = true;
}
float *Sampler::GetLazyValues(Sample *sample, u_int num, u_int pos)
{
	return sample->xD[num] + pos * sample->dxD[num];
}
void Sampler::AddSample(const Sample &sample)
{
	contribBuffer->AddSampleCount(1.f); // TODO: add to the correct buffer groups
	for (u_int i=0; i<sample.contributions.size(); i++) {
			// Radiance - added new use of contributionpool/buffers
			if(!contribBuffer->Add(&sample.contributions[i], 1.f)) {
				contribBuffer = film->scene->contribPool->Next(contribBuffer);
				contribBuffer->Add(&sample.contributions[i], 1.f);
			}
	}
}

// Sample Method Definitions
Sample::Sample(SurfaceIntegrator *surf, VolumeIntegrator *vol,
	const Scene *scene)
{
	stamp = 0;
	sampler = NULL;
	surf->RequestSamples(this, scene);
	vol->RequestSamples(this, scene);
	// Allocate storage for sample pointers
	int nPtrs = n1D.size() + n2D.size() + nxD.size();
	if (!nPtrs) {
		oneD = twoD = xD = NULL;
		return;
	}
	oneD = (float **)AllocAligned(nPtrs * sizeof(float *));
	timexD = (int **)AllocAligned(nxD.size() * sizeof(int *));
	twoD = oneD + n1D.size();
	xD = twoD + n2D.size();
	// Compute total number of sample values needed
	int totSamples = 0;
	int totTime = 0;
	for (u_int i = 0; i < n1D.size(); ++i)
		totSamples += n1D[i];
	for (u_int i = 0; i < n2D.size(); ++i)
		totSamples += 2 * n2D[i];
	for (u_int i = 0; i < nxD.size(); ++i) {
		totSamples += dxD[i] * nxD[i];
		totTime += nxD[i];
	}
	// Allocate storage for sample values
	float *mem = (float *)AllocAligned(totSamples *
		sizeof(float));
	int *tmem = (int *)AllocAligned(totTime * sizeof(int));
	for (u_int i = 0; i < n1D.size(); ++i) {
		oneD[i] = mem;
		mem += n1D[i];
	}
	for (u_int i = 0; i < n2D.size(); ++i) {
		twoD[i] = mem;
		mem += 2 * n2D[i];
	}
	for (u_int i = 0; i < nxD.size(); ++i) {
		xD[i] = mem;
		mem += dxD[i] * nxD[i];
		timexD[i] = tmem;
		tmem += nxD[i];
	}
}

namespace girl
{

// Sampling Function Definitions
 void StratifiedSample1D(const TsPack *tspack, float *samp, int nSamples,
		bool jitter) {
	float invTot = 1.f / nSamples;
	for (int i = 0;  i < nSamples; ++i) {
		float delta = jitter ? tspack->rng->floatValue() : 0.5f;
		*samp++ = (i + delta) * invTot;
	}
}
 void StratifiedSample2D(const TsPack *tspack, float *samp, int nx, int ny,
		bool jitter) {
	float dx = 1.f / nx, dy = 1.f / ny;
	for (int y = 0; y < ny; ++y)
		for (int x = 0; x < nx; ++x) {
			float jx = jitter ? tspack->rng->floatValue() : 0.5f;
			float jy = jitter ? tspack->rng->floatValue() : 0.5f;
			*samp++ = (x + jx) * dx;
			*samp++ = (y + jy) * dy;
		}
}
 void Shuffle(const TsPack *tspack, float *samp, int count, int dims) {
	for (int i = 0; i < count; ++i) {
		u_int other = tspack->rng->uintValue() % count;
		for (int j = 0; j < dims; ++j)
			swap(samp[dims*i + j], samp[dims*other + j]);
	}
}
 void LatinHypercube(const TsPack *tspack, float *samples,
                             int nSamples, int nDim) {
	// Generate LHS samples along diagonal
	float delta = 1.f / nSamples;
	for (int i = 0; i < nSamples; ++i)
		for (int j = 0; j < nDim; ++j)
			samples[nDim * i + j] = (i + tspack->rng->floatValue()) * delta;
	// Permute LHS samples in each dimension
	for (int i = 0; i < nDim; ++i) {
		for (int j = 0; j < nSamples; ++j) {
			u_int other = tspack->rng->uintValue() % nSamples;
			swap(samples[nDim * j + i],
			     samples[nDim * other + i]);
		}
	}
}
 
}//namespace girl

