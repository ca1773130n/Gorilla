/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   reinhard.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - reinhard.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "tonemap.h"
#include "paramset.h"

namespace girl
{

// ReinhardOp Declarations
class GIRLCOREDLL  ReinhardOp : public ToneMap {
public:
	ReinhardOp(float prS, float poS, float b);
	void Map(vector<Color> &xyz, int xRes, int yRes, float maxDisplayY) const;
	static ToneMap *CreateToneMap(const ParamSet &ps);
	
private:
	float pre_scale;
	float post_scale;
	float burn;
};

}//namespace girl

