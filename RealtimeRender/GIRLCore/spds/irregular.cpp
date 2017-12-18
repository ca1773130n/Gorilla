/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   irregular.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - irregular.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// irregular.cpp*
#include "irregular.h"

using namespace girl;

// creates an irregularly sampled SPD
// may "truncate" the edges to fit the new resolution
//  wavelengths   array containing the wavelength of each sample
//  samples       array of sample values at the given wavelengths
//  n             number of samples
//  resolution    resampling resolution (in nm)
IrregularSPD::IrregularSPD(const float* const wavelengths, const float* const samples,
								int n, int resolution, SPDResamplingMethod resamplignMethod) 
								: SPD() {

  int lambdaMin = Ceil2Int(wavelengths[0] / resolution) * resolution;
  int lambdaMax = Floor2Int(wavelengths[n-1] / resolution) * resolution;

  int sn = (lambdaMax - lambdaMin) / resolution + 1;

  vector<float> sam(sn);

  if (resamplignMethod == Linear) {
    int k = 0;
    for (int i = 0; i < sn; i++) {

      float lambda = lambdaMin + i * resolution;

      if (lambda < wavelengths[0] || lambda > wavelengths[n-1]) {
        sam[i] = 0.0;
        continue;
      }

	  for (; k < n; ++k)
		  if (wavelengths[k] >= lambda)
			  break;

	  if (wavelengths[k] == lambda)
		  sam[i] = samples[k];
      else { 
	    float intervalWidth = wavelengths[k] - wavelengths[k - 1];
	    float u = (lambda - wavelengths[k - 1]) / intervalWidth;
	    sam[i] = ((1. - u) * samples[k - 1]) + (u * samples[k]);
      }
    }
  }
  else {
    vector<float> sd(n);

    calc_spline_data(wavelengths, samples, n, &sd[0]);

    int k = 0;
    for (int i = 0; i < sn; i++) {

      float lambda = lambdaMin + i * resolution;

      if (lambda < wavelengths[0] || lambda > wavelengths[n-1]) {
        sam[i] = 0.0;
        continue;
      }

      while (lambda > wavelengths[k+1])
        k++;

      float h = wavelengths[k+1] - wavelengths[k];
      float a = (wavelengths[k+1] - lambda) / h;
      float b = (lambda - wavelengths[k]) / h;

      sam[i] = max(a*samples[k] + b*samples[k+1]+
        ((a*a*a-a)*sd[k] + (b*b*b-b)*sd[k+1])*(h*h)/6.0, 0.);
    }
  }

  init(lambdaMin, lambdaMax, &sam[0], sn);
}

void IrregularSPD::init(float lMin, float lMax, const float* const s, int n) {
  lambdaMin = lMin;
  lambdaMax = lMax;
  delta = (lambdaMax - lambdaMin) / (n-1);
  invDelta = 1.f / delta;
  nSamples = n;

  AllocateSamples(n);

  // Copy samples
  for (int i = 0; i < n; i++)
    samples[i] = s[i];

}

// computes data for natural spline interpolation
// from Numerical Recipes in C
void IrregularSPD::calc_spline_data(const float* const wavelengths,
									const float* const amplitudes, int n, float *spline_data) {
  vector<float> u(n-1);

  // natural spline
  spline_data[0] = u[0] = 0.f;

  for (int i = 1; i <= n-2; i++) {
    float sig = (wavelengths[i] - wavelengths[i-1]) / (wavelengths[i+1] - wavelengths[i-1]);
    float p = sig * spline_data[i-1] + 2.f;
    spline_data[i] = (sig-1.0)/p;
    u[i] = (amplitudes[i+1] - amplitudes[i]) / (wavelengths[i+1] - wavelengths[i]) - 
      (amplitudes[i] - amplitudes[i-1]) / (wavelengths[i] - wavelengths[i-1]);
    u[i] = (6.0*u[i] / (wavelengths[i+1] - wavelengths[i-1]) - sig*u[i-1]) / p;
  }

  float qn, un;
  
  // natural spline
  qn = un = 0.0;
  spline_data[n-1] = (un - qn * u[n-2]) / (qn * spline_data[n-2] + 1.0);
  for (int k = n-2; k >= 0; k--) {
    spline_data[k] = spline_data[k] * spline_data[k+1] + u[k];
  }
}



