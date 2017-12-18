/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   contrast.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - contrast.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// contrast.cpp*
#include "tonemap.h"
#include "paramset.h"

namespace girl
{

// ContrastOp Declarations
class GIRLCOREDLL  ContrastOp : public ToneMap {
public:
	ContrastOp(float day) { displayAdaptationY = day; }
	void Map(vector<Color> &xyz, int xRes, int yRes,
		 float maxDisplayY) const;
	static ToneMap *CreateToneMap(const ParamSet &ps);
private:
	float displayAdaptationY;
};

}//namespace girl
