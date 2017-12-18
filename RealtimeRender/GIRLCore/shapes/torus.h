/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   torus.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - torus.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// torus.cpp*
#include "shape.h"
#include "mc.h"

namespace girl
{

// Torus Declarations
class GIRLCOREDLL  Torus: public Shape {
public:
	// Sphere Public Methods
	Torus(const Transform &o2w, bool ro, float marad, float mirad,
	       float zmin, float zmax, float phiMax);
	BBox ObjectBound() const;
	bool Intersect(const Ray &ray, float *tHit,
	               DifferentialGeometry *dg) const;
	bool IntersectP(const Ray &ray) const;
	float Area() const;
	Point Sample(float u1, float u2, float u3, Normal *ns) const {

		float phi = u1 * phiMax;
		float theta;
		// select inner or outer half based on u2
		theta = thetaMin + u2 * (thetaMax - thetaMin);

		float cosphi = cosf(phi);
		float sinphi = sinf(phi);
		float sintheta = sinf(theta);

		Point p = Point((majorRadius+minorRadius*sintheta)*cosphi,
			(majorRadius+minorRadius*sintheta)*sinphi, minorRadius*cosf(theta));

		Point cp = Point(majorRadius*cosphi, majorRadius*sinphi, 0);

		*ns = Normalize(ObjectToWorld(Normal(p - cp)));
		if (reverseOrientation) *ns *= -1.f;
		return ObjectToWorld(p);
	}

	static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);
private:
	bool FindIntersection(const Ray &ray, float *tHit, Point *pHit, float *phiHit, float *thetaHit) const;

	// Sphere Private Data
	float majorRadius, minorRadius;
	float phiMax;
	float thetaMin, thetaMax;
	// lowest and highest point on torus, used for bounding
	float zmin, zmax;
};

}//namespace girl
