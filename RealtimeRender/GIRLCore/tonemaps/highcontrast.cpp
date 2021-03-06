/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   highcontrast.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - highcontrast.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// highcontrast.cpp*
#include "highcontrast.h"
#include "stats.h"
#include "dynload.h"

using namespace girl;

// HighContrastOp Method Definitions
void HighContrastOp::Map(vector<Color> &xyz, int xRes, int yRes,
		float maxDisplayY) const {
	const int numPixels = xRes * yRes;
	float *lum = new float[numPixels];
	// Find minimum and maximum image luminances
	float minY = INFINITY, maxY = 0.f;
	for (int i = 0; i < numPixels; ++i) {
		lum[i] = xyz[i].c[1] * 683.f;
		minY = min(minY, lum[i]);
		maxY = max(maxY, lum[i]);
	}
	float CYmin = C(minY), CYmax = C(maxY);
	// Build luminance image pyramid
	MIPMapImpl<float, float> pyramid(MIPMAP_EWA, xRes, yRes, lum, 4.f,
		TEXTURE_CLAMP);
	delete[] lum;
	// Apply high contrast tone mapping operator
	ProgressReporter progress(xRes*yRes, "Tone Mapping"); // NOBOOK
	for (int y = 0; y < yRes; ++y) {
		float yc = (float(y) + .5f) / float(yRes);
		for (int x = 0; x < xRes; ++x) {
			float xc = (float(x) + .5f) / float(xRes);
			// Compute local adaptation luminance at $(x,y)$
			float dwidth = 1.f / float(max(xRes, yRes));
			float maxWidth = 32.f / float(max(xRes, yRes));
			float width = dwidth, prevWidth = 0.f;
			float Yadapt;
			float prevlc = 0.f;
			const float maxLocalContrast = .5f;
			while (1) {
				// Compute local contrast at $(x,y)$
				float b0 = pyramid.Lookup(xc, yc, width,
					0.f, 0.f, width);
				float b1 = pyramid.Lookup(xc, yc, 2.f * width,
					0.f, 0.f, 2.f * width);
				float lc = fabsf((b0 - b1) / b0);
				// If maximum contrast is exceeded, compute adaptation luminance
				if (lc > maxLocalContrast) {
					float t = (maxLocalContrast - prevlc) / (lc - prevlc);
					float w = Lerp(t, prevWidth, width);
					Yadapt = pyramid.Lookup(xc, yc, w,
						0.f, 0.f, w);
					break;
				}
				// Increase search region and prepare to compute contrast again
				prevlc = lc;
				prevWidth = width;
				width += dwidth;
				if (width >= maxWidth) {
					Yadapt = pyramid.Lookup(xc, yc, maxWidth,
						0.f, 0.f, maxWidth);
					break;
				}
			}
			// Apply tone mapping based on local adaptation luminance
			xyz[x + y*xRes] *= T(Yadapt, CYmin, CYmax) * 683.f /
				Yadapt;
		}
		progress.Update(xRes); // NOBOOK
	}
	progress.Done(); // NOBOOK
}
ToneMap * HighContrastOp::CreateToneMap(const ParamSet &ps) {
	return new HighContrastOp;
}

static DynamicLoader::RegisterToneMap<HighContrastOp> r("highcontrast");
