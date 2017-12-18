/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   regular.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - regular.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_REGULARSPD_H
#define GIRL_REGULARSPD_H
// regular.h*
#include "girl.h"
#include "color.h"
#include "spectrum.h"
#include "spd.h"

namespace girl
{

// regularly sampled SPD, reconstructed using linear interpolation
class GIRLCOREDLL  RegularSPD : public SPD {
  public:
    RegularSPD() : SPD() {}

    //  creates a regularly sampled SPD
    //  samples    array of sample values
    //  lambdaMin  wavelength (nm) of first sample
    //  lambdaMax  wavelength (nm) of last sample
    //  n          number of samples
    RegularSPD(const float* const samples, float lambdaMin, float lambdaMax, int n) : SPD() {
      init(lambdaMin, lambdaMax, samples, n);
    }

    ~RegularSPD() {}

  protected:
    void init(float lMin, float lMax, const float* const s, int n);

  private:   
  };

  }//namespace girl

#endif // GIRL_REGULARSPD_H
