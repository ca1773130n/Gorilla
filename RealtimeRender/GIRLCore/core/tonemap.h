/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   tonemap.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - tonemap.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_TONEMAP_H
#define GIRL_TONEMAP_H
// tonemap.h*

#include "girl.h"

namespace girl {

// ToneMap Declarations
class GIRLCOREDLL  ToneMap {
public:
	// ToneMap Interface
	virtual ~ToneMap() { }
	virtual void Map(vector<Color> &xyz, int xRes, int yRes, float maxDisplayY) const = 0;
};

}

#endif // GIRL_TONEMAP_H
