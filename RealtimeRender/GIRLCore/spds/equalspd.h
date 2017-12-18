/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   equalspd.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - equalspd.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_EQUALSPD_H
#define GIRL_EQUALSPD_H
// equalspd.h*
#include "girl.h"
#include "color.h"
#include "spectrum.h"
#include "spd.h"

// Equal energy SPD

#define EQ_CACHE_START   380. // precomputed cache starts at wavelength,
#define EQ_CACHE_END     720. // and ends at wavelength
#define EQ_CACHE_SAMPLES 2  // total number of cache samples 

namespace girl
{

  class GIRLCOREDLL  EqualSPD : public SPD {
  public:
    EqualSPD() : SPD() {
		init(1.f);
    }

    EqualSPD(float p) : SPD() {
		init(p);
    }

    ~EqualSPD() {
		FreeSamples();
	}

	void init(float p);

  protected:
    float power;

  };

}//namespace girl

#endif // GIRL_EQUALSPD_H

