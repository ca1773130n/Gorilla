/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   blackbodyspd.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - blackbodyspd.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_BLACKBODYSPD_H
#define GIRL_BLACKBODYSPD_H
// blackbody.h*
#include "girl.h"
#include "color.h"
#include "spectrum.h"
#include "spd.h"

#define BB_CACHE_START   380. // precomputed cache starts at wavelength,
#define BB_CACHE_END     720. // and ends at wavelength
#define BB_CACHE_SAMPLES 256  // total number of cache samples 

namespace girl
{

  class GIRLCOREDLL  BlackbodySPD : public SPD {
  public:
    BlackbodySPD() : SPD() {
		init(6500.f); // default to D65 temperature
    }

    BlackbodySPD(float t) : SPD() {
		init(t);
    }

    ~BlackbodySPD() {
		FreeSamples();
	}

	void init(float t);

  protected:
    float temp;

  };

}//namespace girl

#endif // GIRL_BLACKBODYSPD_H

