/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   paraboloid.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - paraboloid.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// paraboloid.cpp*
#include "shape.h"

namespace girl
{

// Paraboloid Declarations
class GIRLCOREDLL  Paraboloid: public Shape {
public:
	// Paraboloid Public Methods
	Paraboloid(const Transform &o2w, bool ro, float rad,
	           float z0, float z1, float tm );
	BBox ObjectBound() const;
	bool Intersect(const Ray &ray, float *tHit,
	                  DifferentialGeometry *dg) const;
	bool IntersectP(const Ray &ray) const;
	float Area() const;
	
	static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);
protected:
	// Paraboloid Data
	float radius;
	float zmin, zmax;
	float phiMax;
};

}//namespace girl
