/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   sphere.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - sphere.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// sphere.cpp*
#include "shape.h"
#include "mc.h"

namespace girl
{

// Sphere Declarations
class GIRLCOREDLL  Sphere: public Shape {
public:
	// Sphere Public Methods
	Sphere(const Transform &o2w, bool ro, float rad,
	       float zmin, float zmax, float phiMax);
	BBox ObjectBound() const;
	bool Intersect(const Ray &ray, float *tHit,
	               DifferentialGeometry *dg) const;
	bool IntersectP(const Ray &ray) const;
	float Area() const;
	Point Sample(float u1, float u2, float u3, Normal *ns) const {
		Point p = Point(0,0,0) + radius *
			UniformSampleSphere(u1, u2);
		*ns = Normalize(ObjectToWorld(Normal(p.x, p.y, p.z)));
		if (reverseOrientation) *ns *= -1.f;
		return ObjectToWorld(p);
	}
	Point Sample(const Point &p,
			float u1, float u2, float u3, Normal *ns) const {
		// Compute coordinate system for sphere sampling
		Point Pcenter = ObjectToWorld(Point(0,0,0));
		Vector wc = Normalize(Pcenter - p);
		Vector wcX, wcY;
		CoordinateSystem(wc, &wcX, &wcY);
		// Sample uniformly on sphere if \pt is inside it
		if (DistanceSquared(p, Pcenter) - radius*radius < 1e-4f)
			return Sample(u1, u2, u3, ns);
		// Sample sphere uniformly inside subtended cone
		float cosThetaMax = sqrtf(max(0.f, 1.f - radius*radius /
			DistanceSquared(p, Pcenter)));
		DifferentialGeometry dgSphere;
		float thit;
		Point ps;
		Ray r(p,
		      UniformSampleCone(u1, u2, cosThetaMax, wcX, wcY, wc));
		if (!Intersect(r, &thit, &dgSphere)) {
			ps = Pcenter - radius * wc;
		} else {
			ps = r(thit);
		}
		*ns = Normal(Normalize(ps - Pcenter));
		if (reverseOrientation) *ns *= -1.f;
		return ps;
	}
	float Pdf(const Point &p, const Vector &wi) const {
		Point Pcenter = ObjectToWorld(Point(0,0,0));
		// Return uniform weight if point inside sphere
		if (DistanceSquared(p, Pcenter) - radius*radius < 1e-4f)
			return Shape::Pdf(p, wi);
		// Compute general sphere weight
		float cosThetaMax = sqrtf(max(0.f, 1.f - radius*radius /
			DistanceSquared(p, Pcenter)));
		return UniformConePdf(cosThetaMax);
	}
	
	static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);
private:
	// Sphere Private Data
	float radius;
	float phiMax;
	float zmin, zmax;
	float thetaMin, thetaMax;
};

}//namespace girl
