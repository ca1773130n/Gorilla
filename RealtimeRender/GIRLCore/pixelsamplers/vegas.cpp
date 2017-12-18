/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   vegas.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - vegas.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// vegas.cpp*
#include "vegas.h"
#include "error.h"

using namespace girl;

// VegasPixelSampler Method Definitions
VegasPixelSampler::VegasPixelSampler(int xstart, int xend,
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

	// Shuffle elements by randomly exchanging each with one other.
    for (u_int i=0; i<TotalPx; i++) {
		u_int r = Floor2Int( girl::random::floatValueP() * TotalPx );
		// swap
		short int temp = Pxa[i].x; Pxa[i].x = Pxa[r].x; Pxa[r].x = temp;
		temp = Pxa[i].y; Pxa[i].y = Pxa[r].y; Pxa[r].y = temp;
    } 
}

u_int VegasPixelSampler::GetTotalPixels() {
	return TotalPx;
}

bool VegasPixelSampler::GetNextPixel(int &xPos, int &yPos, u_int *use_pos) {
	u_int pos = (*use_pos);
	bool hasMorePixel = true;
	if(pos == TotalPx - 1)
		hasMorePixel = false;

	xPos = Pxa[*use_pos].x;
	yPos = Pxa[*use_pos].y;

	return hasMorePixel;
}
