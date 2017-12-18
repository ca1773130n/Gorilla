/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   vegas.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - vegas.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// vegas.cpp*
#include "sampling.h"
#include "paramset.h"
#include "film.h"

namespace girl
{

// VegasPixelSampler Declarations
class GIRLCOREDLL  VegasPixelSampler : public PixelSampler {
public:
	// VegasPixelSampler Public Methods
	VegasPixelSampler(int xstart, int xend,
	          int ystart, int yend);
	~VegasPixelSampler() {
	}

	u_int GetTotalPixels();
	bool GetNextPixel(int &xPos, int &yPos, u_int *use_pos);

private:
	// VegasPixelSampler Private Data
	u_int TotalPx;

	vector<PxLoc> Pxa; // pixel coordinate cache
};

}//namespace girl
