/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   mitchell.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - mitchell.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_MITCHELL_H
#define GIRL_MITCHELL_H

// mitchell.cpp*
#include "filter.h"
#include "paramset.h"

namespace girl
{

// Mitchell Filter Declarations
class GIRLCOREDLL  MitchellFilter : public Filter {
public:
	// MitchellFilter Public Methods
	MitchellFilter(float b, float c, float xw, float yw)
		: Filter(xw, yw) { B = b; C = c; }
	float Evaluate(float x, float y) const;
	float Mitchell1D(float x) const {
		x = fabsf(2.f * x);
		if (x > 1.f)
			return (((-B/6.f - C) * x + (B + 5.f*C)) * x +
				(-2.f*B - 8.f*C)) * x + (4.f/3.f*B + 4.f*C);
		else
			return ((2.f - 1.5f*B - C) * x +
				(-3.f + 2.f*B + C)) * x*x +
				(1.f - B/3.f);
	}
	
	static Filter *CreateFilter(const ParamSet &ps);
private:
	float B, C;
};

}//namespace girl

#endif

