/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   triangle.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - triangle.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_TRIANGLE_H
#define GIRL_TRIANGLE_H

// triangle.cpp*
#include "filter.h"
#include "paramset.h"

namespace girl
{

// Triangle Filter Declarations
class GIRLCOREDLL  TriangleFilter : public Filter {
public:
	TriangleFilter(float xw, float yw) : Filter(xw, yw) { }
	float Evaluate(float x, float y) const;
	
	static Filter *CreateFilter(const ParamSet &ps);
};

}//namespace girl

#endif

