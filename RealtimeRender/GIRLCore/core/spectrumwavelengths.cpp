/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   spectrumwavelengths.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - spectrumwavelengths.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// Spectrumwavelengths.cpp*
#include "spectrumwavelengths.h"
#include "regular.h"
#include "spd.h"
#include "memory.h"

using namespace girl;

#include "data/rgbE_32.h"

// SpectrumWavelengths Public Methods
SpectrumWavelengths::SpectrumWavelengths() {
	single = false;
	single_w = 0;

	spd_w = new RegularSPD(refrgb2spect_white, 
		refrgb2spect_start, refrgb2spect_end, refrgb2spect_bins);
	spd_w->Scale(refrgb2spect_scale);

	spd_c = new RegularSPD(refrgb2spect_cyan,
		refrgb2spect_start, refrgb2spect_end, refrgb2spect_bins);
	spd_c->Scale(refrgb2spect_scale);

	spd_m = new RegularSPD(refrgb2spect_magenta,
		refrgb2spect_start, refrgb2spect_end, refrgb2spect_bins);
	spd_m->Scale(refrgb2spect_scale);

	spd_y = new RegularSPD(refrgb2spect_yellow,
		refrgb2spect_start, refrgb2spect_end, refrgb2spect_bins);
	spd_y->Scale(refrgb2spect_scale);

	spd_r = new RegularSPD(refrgb2spect_red,
		refrgb2spect_start, refrgb2spect_end, refrgb2spect_bins);
	spd_r->Scale(refrgb2spect_scale);

	spd_g = new RegularSPD(refrgb2spect_green,
		refrgb2spect_start, refrgb2spect_end, refrgb2spect_bins);
	spd_g->Scale(refrgb2spect_scale);

	spd_b = new RegularSPD(refrgb2spect_blue,
		refrgb2spect_start, refrgb2spect_end, refrgb2spect_bins);
	spd_b->Scale(refrgb2spect_scale);

	cie_X = (float *)AllocAligned(WAVELENGTH_SAMPLES * sizeof(float));
	cie_Y = (float *)AllocAligned(WAVELENGTH_SAMPLES * sizeof(float));
	cie_Z = (float *)AllocAligned(WAVELENGTH_SAMPLES * sizeof(float));
}


