/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   rgbrefl.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - rgbrefl.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// rgbrefl.cpp*
#include "rgbrefl.h"
#include "memory.h"

using namespace girl;

#include "data/rgbE_32.h"

void RGBReflSPD::init(RGBColor s) {
	lambdaMin = refrgb2spect_start;
	lambdaMax = refrgb2spect_end;
	int n = refrgb2spect_bins;
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
		AddWeighted(r, refrgb2spect_white);

		if (g <= b)
		{
			AddWeighted(g - r, refrgb2spect_cyan);
			AddWeighted(b - g, refrgb2spect_blue);
		}
		else
		{
			AddWeighted(b - r, refrgb2spect_cyan);
			AddWeighted(g - b, refrgb2spect_green);
		}
	}
	else if (g <= r && g <= b)
	{
		AddWeighted(g, refrgb2spect_white);

		if (r <= b)
		{
			AddWeighted(r - g, refrgb2spect_magenta);
			AddWeighted(b - r, refrgb2spect_blue);
		}
		else
		{
			AddWeighted(b - g, refrgb2spect_magenta);
			AddWeighted(r - b, refrgb2spect_red);
		}
	}
	else // blue <= red && blue <= green
	{
		AddWeighted(b, refrgb2spect_white);

		if (r <= g)
		{
			AddWeighted(r - b, refrgb2spect_yellow);
			AddWeighted(g - r, refrgb2spect_green);
		}
		else
		{
			AddWeighted(g - b, refrgb2spect_yellow);
			AddWeighted(r - g, refrgb2spect_red);
		}
	}

	Scale(refrgb2spect_scale);
	Clamp();
}

