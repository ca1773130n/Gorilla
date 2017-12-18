/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   irregular.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - irregular.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_IRREGULARSPD_H
#define GIRL_IRREGULARSPD_H
// irregular.h*
#include "girl.h"
#include "color.h"
#include "spectrum.h"
#include "regular.h"

namespace girl
{
  // only use spline for regular data
  enum SPDResamplingMethod { Linear, Spline };

  // Irregularly sampled SPD
  // Resampled to a fixed resolution (at construction)
  // using cubic spline interpolation
  class GIRLCOREDLL  IrregularSPD : public SPD {
  public:

    IrregularSPD() : SPD() {}

    // creates an irregularly sampled SPD
    // may "truncate" the edges to fit the new resolution
    //  wavelengths   array containing the wavelength of each sample
    //  samples       array of sample values at the given wavelengths
    //  n             number of samples
    //  resolution    resampling resolution (in nm)
    IrregularSPD(const float* const wavelengths, const float* const samples,
			int n, int resolution = 5, SPDResamplingMethod resamplignMethod = Linear);

    ~IrregularSPD() {}

  protected:
	  void init(float lMin, float lMax, const float* const s, int n);

  private:
    // computes data for natural spline interpolation
    // from Numerical Recipes in C
    void calc_spline_data(const float* const wavelengths,
			const float* const amplitudes, int n, float *spline_data);
  };

}//namespace girl

#endif // GIRL_IRREGULARSPD_H
