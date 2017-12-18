/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   rgbillum.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - rgbillum.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// rgbillum.cpp*
#include "rgbillum.h"
#include "memory.h"

using namespace girl;

#include "data/rgbD65_32.h"

void RGBIllumSPD::init(RGBColor s) {
	lambdaMin = illumrgb2spect_start;
	lambdaMax = illumrgb2spect_end;
	int n = illumrgb2spect_bins;
	delta = (lambdaMax - lambdaMin) / (n-1);
	invDelta = 1.f / delta;
	nSamples = n;

	AllocateSamples(n);

    // Zero out
	for (int i = 0; i < n; i++)
		samples[i] = 0.f;

	float r = s.c[0];
	float g = s.c[1];
	float b = s.c[2];

	if (r <= g && r <= b)
	{
		AddWeighted(r, illumrgb2spect_white);

		if (g <= b)
		{
			AddWeighted(g - r, illumrgb2spect_cyan);
			AddWeighted(b - g, illumrgb2spect_blue);
		}
		else
		{
			AddWeighted(b - r, illumrgb2spect_cyan);
			AddWeighted(g - b, illumrgb2spect_green);
		}
	}
	else if (g <= r && g <= b)
	{
		AddWeighted(g, illumrgb2spect_white);

		if (r <= b)
		{
			AddWeighted(r - g, illumrgb2spect_magenta);
			AddWeighted(b - r, illumrgb2spect_blue);
		}
		else
		{
			AddWeighted(b - g, illumrgb2spect_magenta);
			AddWeighted(r - b, illumrgb2spect_red);
		}
	}
	else // blue <= red && blue <= green
	{
		AddWeighted(b, illumrgb2spect_white);

		if (r <= g)
		{
			AddWeighted(r - b, illumrgb2spect_yellow);
			AddWeighted(g - r, illumrgb2spect_green);
		}
		else
		{
			AddWeighted(g - b, illumrgb2spect_yellow);
			AddWeighted(r - g, illumrgb2spect_red);
		}
	}

	Scale(illumrgb2spect_scale);
	Clamp();
}

