/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   quad.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - quad.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "shape.h"
#include "mc.h"
#include "quadrilateral.h"

namespace girl
{

// Quad Declarations
class GIRLCOREDLL  Quad : public Shape {
public:
	// Quad Public Methods
	Quad(const Transform &o2w, bool ro, int nq, int nv, 
		const int *vi, const Point *P);
	~Quad();
	BBox ObjectBound() const;
	BBox WorldBound() const;
	bool Intersect(const Ray &ray, float *tHit,
	               DifferentialGeometry *dg) const;
	bool IntersectP(const Ray &ray) const;
	float Area() const;
	Point Sample(float u1, float u2, float u3, Normal *Ns) const {
		return quad->Sample(u1, u2, u3, Ns);
	}
	
	static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);
private:
	// Quad Private Data	
	QuadMesh *mesh;
	Quadrilateral *quad;
};

}//namespace girl
