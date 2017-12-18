/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   cylinder.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - cylinder.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// cylinder.cpp*
#include "shape.h"
// Cylinder Declarations

namespace girl
{

class GIRLCOREDLL  Cylinder: public Shape {
public:
	// Cylinder Public Methods
	Cylinder(const Transform &o2w, bool ro, float rad,
	         float zmin, float zmax, float phiMax);
	BBox ObjectBound() const;
	bool Intersect(const Ray &ray, float *tHit,
	               DifferentialGeometry *dg) const;
	bool IntersectP(const Ray &ray) const;
	float Area() const;
	Point Sample(float u1, float u2, float u3, 
			Normal *Ns) const {
		float z = Lerp(u1, zmin, zmax);
		float t = u2 * phiMax;
		Point p = Point(radius * cosf(t), radius * sinf(t), z);
		*Ns = Normalize(ObjectToWorld(Normal(p.x, p.y, 0.)));
		if (reverseOrientation) *Ns *= -1.f;
		return ObjectToWorld(p);
	}
	
	static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);
protected:
	// Cylinder Private Data
	float radius;
	float zmin, zmax;
	float phiMax;
};

}//namespace girl
