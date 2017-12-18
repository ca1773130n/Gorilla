/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   rgbrefl.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - rgbrefl.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_RGBREFLSPD_H
#define GIRL_RGBREFLSPD_H
// rgbrefl.h*
#include "girl.h"
#include "color.h"
#include "spectrum.h"
#include "spd.h"

namespace girl
{

// reflectant SPD, from RGB color, using smits conversion, reconstructed using linear interpolation
  class GIRLCOREDLL  RGBReflSPD : public SPD {
  public:
    RGBReflSPD() : SPD() {
	  init(RGBColor(1.f));
    }

    RGBReflSPD(RGBColor s) : SPD() {
      init(s);
    }

    ~RGBReflSPD() {}

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

#endif // GIRL_RGBREFLSPD_H
