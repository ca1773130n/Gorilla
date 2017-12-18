/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   gaussianspd.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - gaussianspd.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_GAUSSIANSPD_H
#define GIRL_GAUSSIANSPD_H
// gaussian.h*
#include "girl.h"
#include "color.h"
#include "spectrum.h"
#include "spd.h"

// Peak + fallof SPD using gaussian distribution

#define GAUSS_CACHE_START   380. // precomputed cache starts at wavelength,
#define GAUSS_CACHE_END     720. // and ends at wavelength
#define GAUSS_CACHE_SAMPLES 512  // total number of cache samples 

namespace girl
{

  class GIRLCOREDLL  GaussianSPD : public SPD {
  public:
    GaussianSPD() : SPD() {
		init(550.0f, 50.0f, 1.f);
    }

    GaussianSPD(float mean, float width, float refl) : SPD() {
		init(mean, width, refl);
    }

    ~GaussianSPD() {
		FreeSamples();
	}

	void init(float mean, float width, float refl);

  protected:
    float mu, wd, r0;

  };

}//namespace girl

#endif // GIRL_GAUSSIANSPD_H

