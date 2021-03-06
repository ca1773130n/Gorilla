/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   spectrum.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - spectrum.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// RGBColor.cpp*
#include "color.h"
#include "spectrum.h"
#include "spectrumwavelengths.h"
#include "regular.h"
#include "memory.h"

using namespace girl;

XYZColor SWCSpectrum::ToXYZ(const TsPack *tspack) const {
	SpectrumWavelengths *sw = tspack->swl;
	float xyz[3];
	xyz[0] = xyz[1] = xyz[2] = 0.;
	if (sw->single) {
		const int j = sw->single_w;
		xyz[0] = sw->cie_X[j] * c[j];
		xyz[1] = sw->cie_Y[j] * c[j];
		xyz[2] = sw->cie_Z[j] * c[j];
	} else {
		for (unsigned int j = 0; j < WAVELENGTH_SAMPLES; ++j) {
			xyz[0] += sw->cie_X[j] * c[j];
			xyz[1] += sw->cie_Y[j] * c[j];
			xyz[2] += sw->cie_Z[j] * c[j];
		}
		xyz[0] *= inv_WAVELENGTH_SAMPLES;
		xyz[1] *= inv_WAVELENGTH_SAMPLES;
		xyz[2] *= inv_WAVELENGTH_SAMPLES;
	} 

	return XYZColor(xyz);
}

Scalar SWCSpectrum::y(const TsPack *tspack) const {
	SpectrumWavelengths *sw = tspack->swl;
	Scalar y = 0.f;

	if (sw->single) {
		const int j = sw->single_w;
		y = sw->cie_Y[j] * c[j];
	} else {
		for (unsigned int j = 0; j < WAVELENGTH_SAMPLES; ++j) {
			y += sw->cie_Y[j] * c[j];
		}
		y *= inv_WAVELENGTH_SAMPLES;
	}

	return y;
}
Scalar SWCSpectrum::filter(const TsPack *tspack) const
{
	SpectrumWavelengths *sw = tspack->swl;
	Scalar result = 0.f;
	if (sw->single) {
		result = c[sw->single_w];
	} else {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			result += c[i];
		result *= inv_WAVELENGTH_SAMPLES;
	}
	return result;
}

SWCSpectrum::SWCSpectrum(const TsPack *tspack, const SPD *s) {
	SpectrumWavelengths *sw = tspack->swl;
	for (unsigned int j = 0; j < WAVELENGTH_SAMPLES; ++j) {
		c[j] = s->sample(sw->w[j]);
	}
}

SWCSpectrum::SWCSpectrum(const TsPack *tspack, RGBColor s) {
	SpectrumWavelengths *sw = tspack->swl;
	const float r = s.c[0];
	const float g = s.c[1];
	const float b = s.c[2];

	for (unsigned int j = 0; j < WAVELENGTH_SAMPLES; ++j)
		c[j] = 0.;

	if (r <= g && r <= b)
	{
		AddWeighted(r, sw->spect_w);

		if (g <= b)
		{
			AddWeighted(g - r, sw->spect_c);
			AddWeighted(b - g, sw->spect_b);
		}
		else
		{
			AddWeighted(b - r, sw->spect_c);
			AddWeighted(g - b, sw->spect_g);
		}
	}
	else if (g <= r && g <= b)
	{
		AddWeighted(g, sw->spect_w);

		if (r <= b)
		{
			AddWeighted(r - g, sw->spect_m);
			AddWeighted(b - r, sw->spect_b);
		}
		else
		{
			AddWeighted(b - g, sw->spect_m);
			AddWeighted(r - b, sw->spect_r);
		}
	}
	else // blue <= red && blue <= green
	{
		AddWeighted(b, sw->spect_w);

		if (r <= g)
		{
			AddWeighted(r - b, sw->spect_y);
			AddWeighted(g - r, sw->spect_g);
		}
		else
		{
			AddWeighted(g - b, sw->spect_y);
			AddWeighted(r - g, sw->spect_r);
		}
	}
}
