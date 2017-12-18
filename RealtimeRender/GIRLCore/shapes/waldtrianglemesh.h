/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   waldtrianglemesh.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - waldtrianglemesh.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// waldtrianglemesh.cpp*
#include "shape.h"
#include "paramset.h"
#include "mc.h"

namespace girl
{

// WaldTriangleMesh Declarations
class GIRLCOREDLL  WaldTriangleMesh : public Shape {
public:
	// WaldTriangleMesh Public Methods
	WaldTriangleMesh(const Transform &o2w, bool ro,
			int ntris, int nverts, const int *vptr,
			const Point *P, const Normal *N,
			const Vector *S, const float *uv);
	~WaldTriangleMesh();
	BBox ObjectBound() const;
	BBox WorldBound() const;
	bool CanIntersect() const { return false; }
	void Refine(vector<boost::shared_ptr<Primitive> > &refined,
	    		const PrimitiveRefinementHints& refineHints,
	    		boost::shared_ptr<Primitive> thisPtr);
	bool CanSample() const { return false; }
	friend class WaldTriangle;
	template <class T> friend class VertexTexture;

	UINT GetNumVertices() { return nverts; }
	UINT GetNumFaces() { return ntris; }
	Point* GetPoints() { return p; }
	Normal* GetNormals() { return n; }
	int* GetIndices() { return vertexIndex; }
	float* GetUVs() { return uvs; }
	static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);



protected:
	// WaldTriangleMesh Data
	int ntris, nverts;
	int *vertexIndex;
	Point *p;
	Normal *n;
	Vector *s;
	float *uvs;
	vector<boost::shared_ptr<Shape> > triPtrs;
};

class GIRLCOREDLL  WaldTriangle : public Primitive {
public:
	// WaldTriangle Public Methods
	WaldTriangle(WaldTriangleMesh *m, int n);

	BBox ObjectBound() const;
	BBox WorldBound() const;

	bool CanIntersect() const { return true; }
	bool Intersect(const Ray &ray, Intersection* isect) const;
	bool IntersectP(const Ray &ray) const;

	void GetShadingGeometry(const Transform &obj2world,
			const DifferentialGeometry &dg,
			DifferentialGeometry *dgShading) const;

	bool CanSample() const { return true; }
	float Area() const;
	Point Sample(float u1, float u2, float u3, Normal *Ns) const;

private:
	void GetUVs(float uv[3][2]) const {
		if (mesh->uvs) {
			uv[0][0] = mesh->uvs[2*v[0]];
			uv[0][1] = mesh->uvs[2*v[0]+1];
			uv[1][0] = mesh->uvs[2*v[1]];
			uv[1][1] = mesh->uvs[2*v[1]+1];
			uv[2][0] = mesh->uvs[2*v[2]];
			uv[2][1] = mesh->uvs[2*v[2]+1];
		} else {
			uv[0][0] = mesh->p[v[0]].x;
			uv[0][1] = mesh->p[v[0]].y;
			uv[1][0] = mesh->p[v[1]].x;
			uv[1][1] = mesh->p[v[1]].y;
			uv[2][0] = mesh->p[v[2]].x;
			uv[2][1] = mesh->p[v[2]].y;
		}
	}

	// WaldTriangle Data
	WaldTriangleMesh* mesh;
	int *v;

	// Dade - Wald's precomputed values
	enum IntersectionType {
		DOMINANT_X,
		DOMINANT_Y,
		DOMINANT_Z,
		ORTHOGONAL_X,
		ORTHOGONAL_Y,
		ORTHOGONAL_Z,
		DEGENERATE
	};

	IntersectionType intersectionType;
	float nu, nv, nd;
	float bnu, bnv, bnd;
	float cnu, cnv, cnd;

	// Dade - procomputed values for filling the DifferentialGeometry
	Vector dpdu, dpdv;
	Normal normalizedNormal;
};

}//namespace girl
