/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   hilbertpx.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - hilbertpx.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "sampling.h"
#include "paramset.h"
#include "film.h"

namespace girl
{

// Dade - HilbertPixelSampler Declarations
class GIRLCOREDLL  HilbertPixelSampler : public PixelSampler {
public:
	// HilbertPixelSampler Public Methods
	HilbertPixelSampler(
            int xStart, int xEnd,
            int yStart, int yEnd);

	u_int GetTotalPixels();
	bool GetNextPixel(int &xPos, int &yPos, u_int *use_pos);

private:
	void HilberCurve(
		int n,
		int xo, int yo,
		int xd, int yd, int xp, int yp,
		int xEnd, int yEnd);		

	// HilbertPixelSampler Private Data
	u_int TotalPx;

	vector<PxLoc> Pxa; // pixel coordinate cache
};

}//namespace girl
