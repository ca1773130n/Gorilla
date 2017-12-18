/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   hyperboloid.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - hyperboloid.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// hyperboloid.cpp*
#include "shape.h"

namespace girl
{

// Hyperboloid Declarations
class GIRLCOREDLL  Hyperboloid: public Shape {
public:
	// Hyperboloid Public Methods
	Hyperboloid(const Transform &o2w, bool ro,
	            const Point &point1, const Point &point2,
	            float tm);
	BBox ObjectBound() const;
	bool Intersect(const Ray &ray, float *tHit,
	               DifferentialGeometry *dg) const;
	bool IntersectP(const Ray &ray) const;
	float Area() const;
	
	static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);
protected:
	// Hyperboloid Data
	Point p1, p2;
	float zmin, zmax;
	float phiMax;
	float rmax;
	float a, c;
};

}//namespace girl
