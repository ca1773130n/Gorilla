/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   lowdiscrepancypx.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - lowdiscrepancypx.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// lowdiscrepancypx.cpp*
#include "sampling.h"
#include "paramset.h"
#include "film.h"

namespace girl
{

// LowdiscrepancyPixelSampler Declarations
class GIRLCOREDLL  LowdiscrepancyPixelSampler : public PixelSampler {
public:
	// LowdiscrepancyPixelSampler Public Methods
	LowdiscrepancyPixelSampler(int xstart, int xend,
	          int ystart, int yend);
	~LowdiscrepancyPixelSampler() {
	}

	u_int GetTotalPixels();
	bool GetNextPixel(int &xPos, int &yPos, u_int *use_pos);

private:
	// LowdiscrepancyPixelSampler Private Data
	u_int TotalPx;
    // Dade - number of pixel already returned by GetNextPixel()
    u_int pixelCounter;

	int xPixelStart, yPixelStart, xPixelEnd, yPixelEnd;
	u_int xSeed, ySeed;
};

}//namespace girl
