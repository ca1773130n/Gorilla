/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   highcontrast.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - highcontrast.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// highcontrast.cpp*
#include "tonemap.h"
#include "mipmap.h"

namespace girl
{

// HighContrastOp Declarations
class GIRLCOREDLL  HighContrastOp : public ToneMap {
public:
	void Map(vector<Color> &xyz, int xRes, int yRes, float maxDisplayY) const;
			 
	static ToneMap *CreateToneMap(const ParamSet &ps);
private:
	// HighContrastOp Utility Methods
	static float C(float y) {
		if (y < 0.0034f)
			return y / 0.0014f;
		else if (y < 1)
			return 2.4483f + log10f(y / 0.0034f) / 0.4027f;
		else if (y < 7.2444f)
			return 16.563f + (y - 1) / 0.4027f;
		else
			return 32.0693f + log10f(y / 7.2444f) / 0.0556f;
	}
	static float T(float y, float CYmin, float CYmax) {
		return (C(y) - CYmin) / (CYmax - CYmin);
	}
};

}//namespace girl

