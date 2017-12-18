/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   lowdiscrepancypx.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - lowdiscrepancypx.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// lowdiscrepancypx.cpp*
#include "lowdiscrepancypx.h"
#include "error.h"

using namespace girl;

// LowdiscrepancyPixelSampler Method Definitions
LowdiscrepancyPixelSampler::LowdiscrepancyPixelSampler(int xstart, int xend,
		int ystart, int yend) {
	xPixelStart = xstart;
	yPixelStart = ystart;
	xPixelEnd = xend;
	yPixelEnd = yend;
	xSeed = girl::random::uintValueP();
	ySeed = girl::random::uintValueP();

	TotalPx = (xend - xstart) * (yend - ystart);
	pixelCounter = 0;
}

u_int LowdiscrepancyPixelSampler::GetTotalPixels() {
#if defined(WIN32) && !defined(__CYGWIN__)
#undef max // before the use of vaR.max function
#endif
	return std::numeric_limits<unsigned int>::max();
}

bool LowdiscrepancyPixelSampler::GetNextPixel(int &xPos, int &yPos, u_int *use_pos) {
	bool hasMorePixel = true;
	if(pixelCounter == TotalPx) {
		pixelCounter = 0;
		hasMorePixel = false;
	}

	pixelCounter++;

	xPos = xPixelStart + Floor2Int( VanDerCorput( *use_pos, xSeed ) * (xPixelEnd - xPixelStart) );
	yPos = yPixelStart + Floor2Int( Sobol2( *use_pos, ySeed ) * (yPixelEnd - yPixelStart) );

	return hasMorePixel;
}
