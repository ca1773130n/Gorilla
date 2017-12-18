/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   box.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - box.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// box.cpp*

#ifndef GIRL_BOX_H
#define GIRL_BOX_H

#include "filter.h"
#include "paramset.h"

namespace girl
{

// Box Filter Declarations
class GIRLCOREDLL  BoxFilter : public Filter {
public:
	BoxFilter(float xw, float yw) : Filter(xw, yw) { }
	float Evaluate(float x, float y) const;
	
	static Filter *CreateFilter(const ParamSet &ps);
};

}//namespace girl

#endif

