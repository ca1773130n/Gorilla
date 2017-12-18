/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   maxwhite.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - maxwhite.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// maxwhite.cpp*
#include "girl.h"
#include "tonemap.h"
#include "color.h"

namespace girl
{

// MaxWhiteOp Declarations
class GIRLCOREDLL  MaxWhiteOp : public ToneMap {
public:
	// MaxWhiteOp Public Methods
	void Map(vector<Color> &xyz, int xRes, int yRes, float maxDisplayY) const {
		const int numPixels = xRes * yRes;
		// Compute maximum luminance of all pixels
		float maxY = 0.f;
		for (int i = 0; i < numPixels; ++i)
			maxY = max(maxY, xyz[i].c[1]);
		float s = 1.f / maxY;
		for (int i = 0; i < numPixels; ++i)
			xyz[i] *= s;
	}
	
	static ToneMap *CreateToneMap(const ParamSet &ps);
};

}//namespace girl

