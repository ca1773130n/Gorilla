/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   rgbillum.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - rgbillum.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_RGBILLUMSPD_H
#define GIRL_RGBILLUMSPD_H
// rgbillum.h*
#include "girl.h"
#include "color.h"
#include "spectrum.h"
#include "spd.h"

namespace girl
{

// illuminant SPD, from RGB color, using smits conversion, reconstructed using linear interpolation
  class GIRLCOREDLL  RGBIllumSPD : public SPD {
  public:
    RGBIllumSPD() : SPD() {
	  init(RGBColor(1.f));
    }

    RGBIllumSPD(RGBColor s) : SPD() {
      init(s);
    }

    ~RGBIllumSPD() {}

  protected:
	  void AddWeighted(float w, float *c) {
		  for(int i=0; i<nSamples; i++) {
			samples[i] += c[i] * w;
		  }
	  }

	void init(RGBColor s);

  private:   
  };

}//namespace girl

#endif // GIRL_RGBILLUMSPD_H
