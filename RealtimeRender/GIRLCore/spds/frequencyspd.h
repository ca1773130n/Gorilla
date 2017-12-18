/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   frequencyspd.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - frequencyspd.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_FREQUENCYSPD_H
#define GIRL_FREQUENCYSPD_H
// frequencyspd.h*
#include "girl.h"
#include "color.h"
#include "spectrum.h"
#include "spd.h"

// Sin frequency/phase distribution

#define FREQ_CACHE_START   380. // precomputed cache starts at wavelength,
#define FREQ_CACHE_END     720. // and ends at wavelength
#define FREQ_CACHE_SAMPLES 2048  // total number of cache samples 

namespace girl
{

  class GIRLCOREDLL  FrequencySPD : public SPD {
  public:
    FrequencySPD() : SPD() {
		init(0.03f, 0.5f, 1.f);
    }

    FrequencySPD(float freq, float phase, float refl) : SPD() {
		init(freq, phase, refl);
    }

    ~FrequencySPD() {
		FreeSamples();
	}

	void init(float freq, float phase, float refl);

  protected:
    float fq, ph, r0;

  };

}//namespace girl

#endif // GIRL_FREQUENCYSPD_H

