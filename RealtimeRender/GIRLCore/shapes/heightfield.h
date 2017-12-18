/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   heightfield.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - heightfield.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// heightfield.cpp*
#include "shape.h"
#include "paramset.h"
#include "dynload.h"

namespace girl
{

// Heightfield Declarations
class GIRLCOREDLL  Heightfield : public Shape {
public:
	// Heightfield Public Methods
	Heightfield(const Transform &o2w, bool ro, int nu, int nv, const float *zs);
	~Heightfield();
	bool CanIntersect() const;
	void Refine(vector<boost::shared_ptr<Shape> > &refined) const;
	BBox ObjectBound() const;
	
	static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);
private:
	// Heightfield Data
	float *z;
	int nx, ny;
};

}//namespace girl

