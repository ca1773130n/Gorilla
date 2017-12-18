/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   tilepx.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - tilepx.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "sampling.h"
#include "paramset.h"
#include "film.h"

namespace girl
{

// Dade - TilePixelSampler Declarations
class GIRLCOREDLL  TilePixelSampler : public PixelSampler {
    #define TILEPX_SIZE 32

public:
	// TilePixelSampler Public Methods
	TilePixelSampler(
            int xStart, int xEnd,
            int yStart, int yEnd);

	u_int GetTotalPixels();
	bool GetNextPixel(int &xPos, int &yPos, u_int *use_pos);

private:
	// TilePixelSampler Private Data
	u_int TotalPx;

	vector<PxLoc> Pxa; // pixel coordinate cache
};

}//namespace girl
