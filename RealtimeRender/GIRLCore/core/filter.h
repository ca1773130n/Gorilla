/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   filter.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - filter.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_FILTER_H
#define GIRL_FILTER_H
// filter.h*
#include "girl.h"

namespace girl
{

class GIRLCOREDLL  Filter {
public:
	// Filter Interface
	virtual ~Filter() { }
	Filter(float xw, float yw) : xWidth(xw), yWidth(yw),
		invXWidth(1.f / xw), invYWidth(1.f / yw) {}
	virtual float Evaluate(float x, float y) const = 0;
	// Filter Public Data
	const float xWidth, yWidth;
	const float invXWidth, invYWidth;
};

}//namespace girl

#endif // GIRL_FILTER_H
