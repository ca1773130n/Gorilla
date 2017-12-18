/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   film.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - film.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// film.cpp*
#include "film.h"
#include "dynload.h"
#include "paramset.h"
#include "tonemap.h"
#include "stats.h"

namespace girl
{

// Image Pipeline Function Definitions
void ApplyImagingPipeline(vector<Color> &pixels,
	int xResolution, int yResolution, ColorSystem &colorSpace,
	float bloomRadius, float bloomWeight,
	const char *toneMapName, const ParamSet *toneMapParams,
	float gamma, float dither)
{
	const int nPix = xResolution * yResolution ;
	// Possibly apply bloom effect to image
	if (bloomRadius > 0.f && bloomWeight > 0.f) {
		// Compute image-space extent of bloom effect
		const int bloomSupport = Float2Int(bloomRadius *
			max(xResolution, yResolution));
		const int bloomWidth = bloomSupport / 2;
		// Initialize bloom filter table
		vector<float> bloomFilter(bloomWidth * bloomWidth);
		for (int i = 0; i < bloomWidth * bloomWidth; ++i) {
			float dist = sqrtf(float(i)) / float(bloomWidth);
			bloomFilter[i] = powf(max(0.f, 1.f - dist), 4.f);
		}
		// Apply bloom filter to image pixels
		vector<Color> bloomImage(nPix);
		ProgressReporter prog(yResolution, "Bloom filter"); //NOBOOK
		for (int y = 0; y < yResolution; ++y) {
			for (int x = 0; x < xResolution; ++x) {
				// Compute bloom for pixel _(x,y)_
				// Compute extent of pixels contributing bloom
				int x0 = max(0, x - bloomWidth);
				int x1 = min(x + bloomWidth, xResolution - 1);
				int y0 = max(0, y - bloomWidth);
				int y1 = min(y + bloomWidth, yResolution - 1);
				int offset = y * xResolution + x;
				float sumWt = 0.;
				for (int by = y0; by <= y1; ++by) {
					for (int bx = x0; bx <= x1; ++bx) {
						// Accumulate bloom from pixel $(bx,by)$
						int dx = x - bx, dy = y - by;
						if (dx == 0 && dy == 0) continue;
						int dist2 = dx*dx + dy*dy;
						if (dist2 < bloomWidth * bloomWidth) {
							int bloomOffset = bx + by * xResolution;
							float wt = bloomFilter[dist2];
							sumWt += wt;
							bloomImage[offset].AddWeighted(wt, pixels[bloomOffset]);
						}
					}
				}
				bloomImage[offset] /= sumWt;
			}
			prog.Update(); //NOBOOK
		}
		prog.Done(); //NOBOOK
		// Mix bloom effect into each pixel
		for (int i = 0; i < nPix; ++i)
			pixels[i] = Lerp(bloomWeight, pixels[i], bloomImage[i]);
	}
	// Apply tone reproduction to image
	ToneMap *toneMap = NULL;
	if (toneMapName) {
		toneMap = MakeToneMap(toneMapName,
			toneMapParams ? *toneMapParams : ParamSet());
		if (toneMap) {
			toneMap->Map(pixels, xResolution, yResolution, 100.f);
			delete toneMap;
		}
	}
	// Convert to RGB
	const float invGamma = 1.f / gamma;
	for (int i = 0; i < nPix; ++i) {
		pixels[i] = colorSpace.ToRGBConstrained(XYZColor(pixels[i].c));
		// Do gamma correction
		pixels[i] = pixels[i].Pow(invGamma);
	}
	// Dither image
	if (dither > 0.f)
		for (int i = 0; i < nPix; ++i)
			pixels[i] += 2.f * dither * (girl::random::floatValueP() - .5f);
}

}//namespace girl

