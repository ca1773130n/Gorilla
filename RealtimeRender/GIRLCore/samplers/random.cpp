/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   random.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - random.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// random.cpp*
#include "random.h"
#include "vegas.h"
//#include "randompx.h"
#include "lowdiscrepancypx.h"
#include "linear.h"
#include "tilepx.h"
#include "scene.h"
#include "hilbertpx.h"
#include "dynload.h"
#include "error.h"

using namespace girl;

// Girl (copy) constructor
RandomSampler* RandomSampler::clone() const
 {
   return new RandomSampler(*this);
 }

RandomSampler::RandomSampler(int xstart, int xend,
                             int ystart, int yend, int ps, string pixelsampler)
        : Sampler(xstart, xend, ystart, yend, ps)
{
    xPos = xPixelStart;
    yPos = yPixelStart;
    pixelSamples = ps;

	init = true;

	// Initialize PixelSampler
	if(pixelsampler == "vegas")
		pixelSampler = new VegasPixelSampler(xstart, xend, ystart, yend);
	else if(pixelsampler == "lowdiscrepancy")
		pixelSampler = new LowdiscrepancyPixelSampler(xstart, xend, ystart, yend);
//	else if(pixelsampler == "random")
//		pixelSampler = new RandomPixelSampler(xstart, xend, ystart, yend);
    else if((pixelsampler == "tile") || (pixelsampler == "grid"))
		pixelSampler = new TilePixelSampler(xstart, xend, ystart, yend);
	else if(pixelsampler == "hilbert")
		pixelSampler = new HilbertPixelSampler(xstart, xend, ystart, yend);
	else
		pixelSampler = new LinearPixelSampler(xstart, xend, ystart, yend);

	TotalPixels = pixelSampler->GetTotalPixels();

	// Get storage for a pixel's worth of stratified samples
	imageSamples = (float *)AllocAligned(7 * pixelSamples *
		sizeof(float));
	lensSamples = imageSamples + 2 * pixelSamples;
	timeSamples = lensSamples +  2 * pixelSamples;
	wavelengthsSamples = timeSamples + pixelSamples;
	singleWavelengthSamples = wavelengthsSamples + pixelSamples;

	samplePos = pixelSamples;
}

RandomSampler::~RandomSampler()
{
	FreeAligned(imageSamples);
}

// return TotalPixels so scene shared thread increment knows total sample positions
u_int RandomSampler::GetTotalSamplePos()
{
	return TotalPixels;
}

bool RandomSampler::GetNextSample(Sample *sample, u_int *use_pos)
{
	sample->sampler = this;

	if(init) {
		init = false;
		// Fetch first contribution buffer from pool
		contribBuffer = film->scene->contribPool->Next(NULL);
	}

	// Compute new set of samples if needed for next pixel
	bool haveMoreSample = true;
	if (samplePos == pixelSamples) {
		// fetch next pixel from pixelsampler
		if(!pixelSampler->GetNextPixel(xPos, yPos, use_pos)) {
			// Dade - we are at a valid checkpoint where we can stop the
			// rendering. Check if we have enough samples per pixel in the film.
			if (film->enoughSamplePerPixel) {
				// Dade - pixelSampler->renderingDone is shared among all rendering threads
				pixelSampler->renderingDone = true;
				haveMoreSample = false;
			}
		} else
			haveMoreSample = (!pixelSampler->renderingDone);

		for (int i = 0; i < 7 * pixelSamples; ++i) {
			imageSamples[i] = tspack->rng->floatValue();
		}

		samplePos = 0;
	}
	// reset so scene knows to increment
	if (samplePos >= pixelSamples-1)
		*use_pos = -1;
	// Return next \mono{RandomSampler} sample point
	sample->imageX = xPos + imageSamples[2*samplePos];
	sample->imageY = yPos + imageSamples[2*samplePos+1];
	sample->lensU = lensSamples[2*samplePos];
	sample->lensV = lensSamples[2*samplePos+1];
	sample->time = timeSamples[samplePos];
	sample->wavelengths = wavelengthsSamples[samplePos];
	sample->singleWavelength = singleWavelengthSamples[samplePos];
	// Generate stratified samples for integrators
	for (u_int i = 0; i < sample->n1D.size(); ++i) {
		for (u_int j = 0; j < sample->n1D[i]; ++j)
			sample->oneD[i][j] = tspack->rng->floatValue();
	}
	for (u_int i = 0; i < sample->n2D.size(); ++i) {
		for (u_int j = 0; j < 2*sample->n2D[i]; ++j)
			sample->twoD[i][j] = tspack->rng->floatValue();
	}

	++samplePos;

	return haveMoreSample;
}

float *RandomSampler::GetLazyValues(Sample *sample, u_int num, u_int pos)
{
	float *data = sample->xD[num] + pos * sample->dxD[num];
	for (u_int i = 0; i < sample->dxD[num]; ++i)
		data[i] = tspack->rng->floatValue();
	return data;
}

Sampler* RandomSampler::CreateSampler(const ParamSet &params, const Film *film)
{
	int nsamp = params.FindOneInt("pixelsamples", -1);
	// for backwards compatibility
	if (nsamp < 0) {
		girlError(GIRL_NOERROR, GIRL_WARNING, 
			"Parameters 'xsamples' and 'ysamples' are deprecated, use 'pixelsamples' instead");
		int xsamp = params.FindOneInt("xsamples", 2);
		int ysamp = params.FindOneInt("ysamples", 2);
		nsamp = xsamp*ysamp;
	}

	string pixelsampler = params.FindOneString("pixelsampler", "vegas");
    int xstart, xend, ystart, yend;
    film->GetSampleExtent(&xstart, &xend, &ystart, &yend);
    return new RandomSampler(xstart, xend,
                             ystart, yend,
                             nsamp, pixelsampler);
}

static DynamicLoader::RegisterSampler<RandomSampler> r("random");
