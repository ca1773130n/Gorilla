/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   linear.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - linear.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// linear.cpp*
#include "sampling.h"
#include "paramset.h"
#include "film.h"

namespace girl
{

// LinearPixelSampler Declarations
class GIRLCOREDLL  LinearPixelSampler : public PixelSampler {
public:
	// LinearPixelSampler Public Methods
	LinearPixelSampler(int xstart, int xend,
	          int ystart, int yend);

	u_int GetTotalPixels();
	bool GetNextPixel(int &xPos, int &yPos, u_int *use_pos);

private:
	// LinearPixelSampler Private Data
	u_int TotalPx;

	vector<PxLoc> Pxa; // pixel coordinate cache
};

}//namespace girl
