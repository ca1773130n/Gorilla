/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   linear.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - linear.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// linear.cpp*
#include "linear.h"
#include "error.h"

using namespace girl;

// LinearPixelSampler Method Definitions
LinearPixelSampler::LinearPixelSampler(int xstart, int xend,
		int ystart, int yend) {
	int xPos = xstart;
	int yPos = ystart;
	// fill Pxa array in film pixel order
	short int x = (unsigned short int) xPos;
	short int y = (unsigned short int) yPos;
	TotalPx = 0;
	while(true) {
		PxLoc px;
		px.x = x; px.y = y;
		Pxa.push_back(px);
		x++;
		TotalPx++;
		if(x == xend) {
			x = xstart;
			y++;
			if(y == yend)
				break;
		}
	}
}

u_int LinearPixelSampler::GetTotalPixels() {
	return TotalPx;
}

bool LinearPixelSampler::GetNextPixel(int &xPos, int &yPos, u_int *use_pos) {
	u_int pos = (*use_pos);
	bool hasMorePixel = true;
	if(pos == TotalPx - 1)
		hasMorePixel = false;

	xPos = Pxa[*use_pos].x;
	yPos = Pxa[*use_pos].y;

	return hasMorePixel;
}
