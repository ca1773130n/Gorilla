/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   quadrilateral.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - quadrilateral.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "shape.h"
#include "mc.h"

namespace girl
{

// minimalistic mesh class for now
class GIRLCOREDLL  QuadMesh : public Shape  {
public:
	QuadMesh(const Transform &o2w, bool ro, int nq, int nv, 
		const int *vi, const Point *P) 
		: Shape(o2w, ro), nquads(nq), nverts(nv) {				
		
	    idx = new int[3 * nquads];
		memcpy(idx, vi, 4 * nquads * sizeof(int));

		p = new Point[nverts];
		// Transform mesh vertices to world space
		for (int i  = 0; i < nverts; ++i)
			p[i] = ObjectToWorld(P[i]);	

		uvs = NULL;
	}

	~QuadMesh() {
		delete[] idx;
		delete[] p;
	}

	BBox ObjectBound() const;

	int nquads, nverts;
	int *idx;
	Point *p;
	float *uvs;
private:

};

// Quadrilateral Declarations
// assumes points form a strictly convex, planar quad
class GIRLCOREDLL  Quadrilateral : public Shape {
public:
	// Quadrilateral Public Methods
	Quadrilateral(const Transform &o2w, bool ro, QuadMesh *m, int *indices);
	~Quadrilateral();
	BBox ObjectBound() const;
	BBox WorldBound() const;
	bool Intersect(const Ray &ray, float *tHit,
	               DifferentialGeometry *dg) const;
	bool IntersectP(const Ray &ray) const;
	void GetUVs(float uv[4][2]) const {		
		if (mesh->uvs) {
			uv[0][0] = mesh->uvs[2*idx[0]];
			uv[0][1] = mesh->uvs[2*idx[0]+1];
			uv[1][0] = mesh->uvs[2*idx[1]];
			uv[1][1] = mesh->uvs[2*idx[1]+1];
			uv[2][0] = mesh->uvs[2*idx[2]];
			uv[2][1] = mesh->uvs[2*idx[2]+1];
			uv[3][0] = mesh->uvs[2*idx[3]];
			uv[3][1] = mesh->uvs[2*idx[3]+1];
		} else {
			//uv[0][0] = mesh->p[idx[0]].x;
			//uv[0][1] = mesh->p[idx[0]].y;
			//uv[1][0] = mesh->p[idx[1]].x;
			//uv[1][1] = mesh->p[idx[1]].y;
			//uv[2][0] = mesh->p[idx[2]].x;
			//uv[2][1] = mesh->p[idx[2]].y;
			//uv[3][0] = mesh->p[idx[3]].x;
			//uv[3][1] = mesh->p[idx[3]].y;
			uv[0][0] = 0.f;
			uv[0][1] = 0.f;
			uv[1][0] = 1.f;
			uv[1][1] = 0.f;
			uv[2][0] = 1.f;
			uv[2][1] = 1.f;
			uv[3][0] = 0.f;
			uv[3][1] = 1.f;
		}
	}
	float Area() const;
	Point Sample(float u1, float u2, float u3, Normal *Ns) const {
		Point p;

		const Point &p0 = mesh->p[idx[0]];
		const Point &p1 = mesh->p[idx[1]];
		const Point &p2 = mesh->p[idx[2]];
	    const Point &p3 = mesh->p[idx[3]];

		p = (1.f-u1)*(1.f-u2)*p0 + u1*(1.f-u2)*p1
			+u1*u2*p2 + (1.f-u1)*u2*p3;

		Vector e0 = p1 - p0;
		Vector e1 = p2 - p0;

		*Ns = Normalize(Normal(Cross(e0, e1)));
		if (reverseOrientation) *Ns *= -1.f;
		return p;
	}
	
private:
	// Quadrilateral Private Data
	int *idx;
	QuadMesh *mesh;
};

}//namespace girl
