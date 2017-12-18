/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   exrio.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - exrio.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_EXRIO_H
#define GIRL_EXRIO_H
// exrio.h*

#include "girl.h"
#include "color.h"

namespace girl
{

void WriteRGBAImage(const string &name,
	float *pixels, float *alpha, int XRes, int YRes,
	int totalXRes, int totalYRes, int xOffset, int yOffset);
void WriteRGBAImageFloat(const string &name,
	vector<Color> &pixels, vector<float> &alpha, int XRes, int YRes,
	int totalXRes, int totalYRes, int xOffset, int yOffset);

}

#endif // GIRL_EXRIO_H

