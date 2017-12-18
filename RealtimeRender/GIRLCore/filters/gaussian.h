/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   gaussian.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - gaussian.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_GAUSSIAN_H
#define GIRL_GAUSSIAN_H

// gaussian.cpp*
#include "filter.h"
#include "paramset.h"

namespace girl
{

// Gaussian Filter Declarations
class GIRLCOREDLL  GaussianFilter : public Filter {
public:
	// GaussianFilter Public Methods
	GaussianFilter(float xw, float yw, float a)
		: Filter(xw, yw) {
		alpha = a;
		expX = expf(-alpha * xWidth * xWidth);
		expY = expf(-alpha * yWidth * yWidth);
	}
	float Evaluate(float x, float y) const;
	
	static Filter *CreateFilter(const ParamSet &ps);
private:
	// GaussianFilter Private Data
	float alpha;
	float expX, expY;
	// GaussianFilter Utility Functions
	float Gaussian(float d, float expv) const {
		return max(0.f, expf(-alpha * d * d) - expv);
	}
};

}//namespace girl

#endif
