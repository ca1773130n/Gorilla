/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   disk.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - disk.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "shape.h"
#include "mc.h"

namespace girl
{

// Disk Declarations
class GIRLCOREDLL  Disk : public Shape {
public:
	// Disk Public Methods
	Disk(const Transform &o2w, bool ro, float height,
	     float radius, float innerRadius, float phiMax);
	BBox ObjectBound() const;
	bool Intersect(const Ray &ray, float *tHit,
	               DifferentialGeometry *dg) const;
	bool IntersectP(const Ray &ray) const;
	float Area() const;
	Point Sample(float u1, float u2, float u3, Normal *Ns) const {
		Point p;
		ConcentricSampleDisk(u1, u2, &p.x, &p.y);
		p.x *= radius;
		p.y *= radius;
		p.z = height;
		*Ns = Normalize(ObjectToWorld(Normal(0,0,1)));
		if (reverseOrientation) *Ns *= -1.f;
		return ObjectToWorld(p);
	}
	
	static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);
private:
	// Disk Private Data
	float height, radius, innerRadius, phiMax;
};

}//namespace girl

