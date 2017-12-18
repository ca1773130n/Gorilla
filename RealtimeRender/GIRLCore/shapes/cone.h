/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   cone.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - cone.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// cone.cpp*
#include "shape.h"
// Cone Declarations

namespace girl
{

class GIRLCOREDLL  Cone: public Shape {
public:
	// Cone Public Methods
	Cone(const Transform &o2w, bool ro,
	     float height, float rad, float tm );
	BBox ObjectBound() const;
	bool Intersect(const Ray &ray, float *tHit,
	               DifferentialGeometry *dg) const;
	bool IntersectP(const Ray &ray) const;
	float Area() const;
	
	static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);

protected:
	// Cone Data
	float radius, height, phiMax;
};

}//namespace girl

