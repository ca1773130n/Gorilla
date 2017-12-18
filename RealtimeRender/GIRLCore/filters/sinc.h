/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   sinc.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - sinc.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_SINC_H
#define GIRL_SINC_H

// sinc.cpp*
#include "filter.h"
#include "paramset.h"

namespace girl
{

// Sinc Filter Declarations
class GIRLCOREDLL  LanczosSincFilter : public Filter {
public:
	LanczosSincFilter(float xw, float yw, float t) : Filter(xw, yw), tau(t)
	{}
	float Evaluate(float x, float y) const;
	
	static Filter *CreateFilter(const ParamSet &ps);
private:
	float tau;
	float Sinc1D(float x) const;
};

}//namespace girl

#endif

