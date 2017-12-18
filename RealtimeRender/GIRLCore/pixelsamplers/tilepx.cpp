/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   tilepx.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - tilepx.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "tilepx.h"
#include "error.h"

using namespace girl;

// TilePixelSampler Method Definitions
TilePixelSampler::TilePixelSampler(
        int xStart, int xEnd,
        int yStart, int yEnd) {
    // Dade - debugging code
    //std::stringstream ss;
    //ss << "xstart: " << xstart << " xend: " << xend <<
    //        " ystart: " << ystart << " yend: " << yend;
    //girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

    int xSize = xEnd - xStart;
    int ySize = yEnd - yStart;

    int tileXSize = xSize / TILEPX_SIZE + ((xSize % TILEPX_SIZE == 0) ? 0 : 1);
    int tileYSize = ySize / TILEPX_SIZE + ((ySize % TILEPX_SIZE == 0) ? 0 : 1);
    
    // Dade - debugging code
    //ss.str("");
    //ss << "tileXSize: " << tileXSize << " tileYSize: " << tileYSize;
    //girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

    TotalPx = 0;
    for(int yg = 0; yg < tileYSize; yg++) {
        for(int xg = 0; xg < tileXSize; xg++) {
            for(int y = yStart +  yg * TILEPX_SIZE; y < yStart + (yg + 1) * TILEPX_SIZE; y++) {
                for(int x = xStart + xg * TILEPX_SIZE; x < xStart + (xg + 1) * TILEPX_SIZE; x++) {
                    if((x <= xEnd) && (y <= yEnd)) {
                        PxLoc px;
                        px.x = x; px.y = y;
                        Pxa.push_back(px);
                        TotalPx++;
                    }
                }
            }
        }
    }
}

u_int TilePixelSampler::GetTotalPixels() {
	return TotalPx;
}

bool TilePixelSampler::GetNextPixel(int &xPos, int &yPos, u_int *use_pos) {
	u_int pos = (*use_pos);
	bool hasMorePixel = true;
	if(pos == TotalPx - 1)
		hasMorePixel = false;

	xPos = Pxa[pos].x;
	yPos = Pxa[pos].y;

    return hasMorePixel;
}
