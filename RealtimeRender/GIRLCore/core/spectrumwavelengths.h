/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   spectrumwavelengths.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - spectrumwavelengths.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_RGBColorWAVELENGTHS_H
#define GIRL_RGBColorWAVELENGTHS_H
// Spectrumwavelengths.h*
#include "girl.h"
#include "spectrum.h"
#include "spd.h"

#include "data/xyzbasis.h"

namespace girl
{

class GIRLCOREDLL 	SpectrumWavelengths {
public:

	// SpectrumWavelengths Public Methods
	SpectrumWavelengths();

	inline void Sample(float u1, float u2) {
		single = false;
		single_w = Floor2Int(u2 * WAVELENGTH_SAMPLES);

		// Sample new stratified wavelengths and precompute RGB/XYZ data
		const float offset = float(WAVELENGTH_END - WAVELENGTH_START) * inv_WAVELENGTH_SAMPLES;
		float waveln = WAVELENGTH_START + u1 * offset;
		for (u_int i = 0; i < WAVELENGTH_SAMPLES; ++i) {
			// Interpolate RGB Conversion SPDs
			spect_w.c[i] = spd_w->sample(waveln);
			spect_c.c[i] = spd_c->sample(waveln);
			spect_m.c[i] = spd_m->sample(waveln);
			spect_y.c[i] = spd_y->sample(waveln);
			spect_r.c[i] = spd_r->sample(waveln);
			spect_g.c[i] = spd_g->sample(waveln);
			spect_b.c[i] = spd_b->sample(waveln);
			// Interpolate XYZ Conversion weights
			const float w0 = waveln - CIEstart;
			int i0 = Floor2Int(w0);
			const float b0 = w0 - i0;
			cie_X[i] = Lerp(b0, CIE_X[i0], CIE_X[i0 + 1]);
			cie_Y[i] = Lerp(b0, CIE_Y[i0], CIE_Y[i0 + 1]);
			cie_Z[i] = Lerp(b0, CIE_Z[i0], CIE_Z[i0 + 1]);
			w[i] = waveln;
			waveln += offset;
		}
	}

	inline float SampleSingle() {
		single = true;
		return w[single_w];
	}

	float w[WAVELENGTH_SAMPLES]; // Wavelengths in nm

	bool single; // Split to single
	int  single_w; // Chosen single wavelength bin

	SWCSpectrum spect_w, spect_c, spect_m;	// white, cyan, magenta
	SWCSpectrum spect_y, spect_r, spect_g;	// yellow, red, green
	SWCSpectrum spect_b;	// blue

	float *cie_X, *cie_Y, *cie_Z; // CIE XYZ weights

private:
	SPD *spd_w, *spd_c, *spd_m, *spd_y,
		*spd_r, *spd_g, *spd_b;
};


}//namespace girl

#endif // GIRL_RGBColorWAVELENGTHS_H
