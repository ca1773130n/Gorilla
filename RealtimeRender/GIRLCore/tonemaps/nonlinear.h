/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   nonlinear.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - nonlinear.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// nonlinear.cpp*
#include "tonemap.h"
#include "paramset.h"

namespace girl
{

// NonLinearOp Declarations
class GIRLCOREDLL  NonLinearOp : public ToneMap {
public:
	// NonLinearOp Public Methods
	NonLinearOp(float my) { maxY = my; }
	void Map(vector<Color> &xyz, int xRes, int yRes, float maxDisplayY) const {
		const int numPixels = xRes * yRes;
		float invY2;
		if (maxY <= 0.f) {
			// Compute world adaptation luminance, _Ywa_
			float Ywa = 0.;
			for (int i = 0; i < numPixels; ++i)
				if (xyz[i].c[1] > 0) Ywa += logf(xyz[i].c[1]);
			Ywa = expf(Ywa / (xRes * yRes));
			invY2 = 1.f / (Ywa * Ywa);
		}
		else
			invY2 = 1.f / (maxY * maxY);
		for (int i = 0; i < numPixels; ++i) {
			float ys = xyz[i].c[1];
			xyz[i] *= (1.f + ys * invY2) / (1.f + ys);
		}
	}
	
	static ToneMap *CreateToneMap(const ParamSet &ps);
private:
	float maxY;
};

}//namespace girl

