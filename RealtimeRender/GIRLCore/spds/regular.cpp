/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   regular.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - regular.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// regular.cpp*
#include "regular.h"
#include "memory.h"

using namespace girl;

void RegularSPD::init(float lMin, float lMax, const float* const s, int n) {
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

