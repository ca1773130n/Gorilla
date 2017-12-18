/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   mesh.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - mesh.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "shape.h"
#include "paramset.h"

namespace girl
{

class GIRLCOREDLL  Mesh : public Shape {
public:
	enum MeshTriangleType { TRI_WALD, TRI_BARY, TRI_AUTO };
	enum MeshQuadType { QUAD_QUADRILATERAL };
	enum MeshAccelType { ACCEL_KDTREE, ACCEL_GRID, ACCEL_NONE, ACCEL_AUTO };
	enum MeshSubdivType { SUBDIV_LOOP };

	Mesh(const Transform &o2w, bool ro,
			MeshAccelType acceltype,
			int nv, const Point *P, const Normal *N, const float *UV,
			MeshTriangleType tritype, int trisCount, const int *tris,
			MeshQuadType quadtype, int nquadsCount, const int *quads,
			MeshSubdivType subdivType, int nsubdivlevels, 
			boost::shared_ptr<Texture<float> > displacementMap, 
			float displacementMapScale, float displacementMapOffset, 
			bool displacementMapNormalSmooth, bool displacementMapSharpBoundary);
	~Mesh();

	BBox ObjectBound() const;
	BBox WorldBound() const;
	bool CanIntersect() const { return false; }
	void Refine(vector<boost::shared_ptr<Primitive> > &refined,
			const PrimitiveRefinementHints &refineHints,
			boost::shared_ptr<Primitive> thisPtr);
	bool CanSample() const { return false; }

	friend class MeshWaldTriangle;
	friend class MeshBaryTriangle;
	friend class MeshQuadrilateral;

	static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);

	class GIRLCOREDLL  BaryMesh {
	public:
		static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);
	};

	class GIRLCOREDLL  WaldMesh {
	public:
		static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);
	};

	class GIRLCOREDLL  LoopMesh {
	public:
		static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);
	};

	int GetNumVertices() { return nverts; }
	int GetNumFaces() { return ntris; }
	Point* GetPoints() { return p; }
	Normal* GetNormals() { return n; }
	float* GetUVs() { return uvs; }
	int* GetIndices() { return triVertexIndex; }

protected:
	// Lotus - refinement data
	MeshAccelType accelType;

	// Dade - vertices data
	int nverts;
	Point *p;
	Normal *n;
	float *uvs;

	// Dade - triangle data
	MeshTriangleType triType;
	int ntris;
	int *triVertexIndex;

	// Dade - quad data
	MeshQuadType quadType;
	int nquads;
	int *quadVertexIndex;

	// Lotus - subdivision data
	bool mustSubdivide;
	int nSubdivLevels;
	MeshSubdivType subdivType;
	// optional displacement map
	boost::shared_ptr<Texture<float> > displacementMap;
	float displacementMapScale;
	float displacementMapOffset;
	bool displacementMapNormalSmooth, displacementMapSharpBoundary;
};

//------------------------------------------------------------------------------
// Triangle shapes
//------------------------------------------------------------------------------

class GIRLCOREDLL  MeshWaldTriangle : public Primitive {
public:
	// WaldTriangle Public Methods
	MeshWaldTriangle(const Mesh *m, int n);

	BBox ObjectBound() const;
	BBox WorldBound() const;

	bool CanIntersect() const { return true; }
	bool Intersect(const Ray &ray, Intersection *isect) const;
	bool IntersectP(const Ray &ray) const;

	void GetShadingGeometry(const Transform &obj2world,
			const DifferentialGeometry &dg,
			DifferentialGeometry *dgShading) const;

	bool CanSample() const { return true; }
	float Area() const;
	Point Sample(float u1, float u2, float u3, Normal *Ns) const;

	bool isDegenerate() const;
private:
	void GetUVs(float uv[3][2]) const {
		if (mesh->uvs) {
			uv[0][0] = mesh->uvs[2 * v[0]];
			uv[0][1] = mesh->uvs[2 * v[0] + 1];
			uv[1][0] = mesh->uvs[2 * v[1]];
			uv[1][1] = mesh->uvs[2 * v[1] + 1];
			uv[2][0] = mesh->uvs[2 * v[2]];
			uv[2][1] = mesh->uvs[2 * v[2] + 1];
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
	const Mesh* mesh;
	const int *v;

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

	// Dade - precomputed values for filling the DifferentialGeometry
	Vector dpdu, dpdv;
	Normal normalizedNormal;
};

class GIRLCOREDLL  MeshBaryTriangle : public Primitive {
public:
    // BaryTriangle Public Methods
    MeshBaryTriangle(const Mesh *m, int n)
		: mesh(m), v(&(mesh->triVertexIndex[3 * n]))
	{
    }

    BBox ObjectBound() const;
    BBox WorldBound() const;

    bool CanIntersect() const { return true; }
    bool Intersect(const Ray &ray, Intersection *isect) const;
    bool IntersectP(const Ray &ray) const;

    void GetShadingGeometry(const Transform &obj2world,
            const DifferentialGeometry &dg,
            DifferentialGeometry *dgShading) const;

    bool CanSample() const { return true; }
    float Area() const;
    Point Sample(float u1, float u2, Normal *Ns) const;

	bool isDegenerate() const {
		return false; //TODO check degenerate
	}

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

	// BaryTriangle Data
	const Mesh *mesh;
	const int *v;
};

//------------------------------------------------------------------------------
// Quad shapes
//------------------------------------------------------------------------------

// Quadrilateral Declarations
// assumes points form a strictly convex, planar quad
class GIRLCOREDLL  MeshQuadrilateral : public Primitive {
public:
	// Quadrilateral Public Methods
	MeshQuadrilateral(const Mesh *m, int n);

	BBox ObjectBound() const;
	BBox WorldBound() const;

	bool CanIntersect() const { return true; }
	bool Intersect(const Ray &ray, Intersection *isect) const;
	bool IntersectP(const Ray &ray) const;

	void GetShadingGeometry(const Transform &obj2world,
            const DifferentialGeometry &dg,
            DifferentialGeometry *dgShading) const;

	bool CanSample() const { return true; }
	float Area() const;
	Point Sample(float u1, float u2, Normal *Ns) const {
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
		if (mesh->reverseOrientation ^ mesh->transformSwapsHandedness)
			*Ns *= -1.f;
		return p;
	}

	bool isDegenerate() const {
		return false; //TODO check degenerate
	}

	static bool IsPlanar(const Point &p0, const Point &p1, const Point &p2, const Point &p3);
	static bool IsDegenerate(const Point &p0, const Point &p1, const Point &p2, const Point &p3);
	static bool IsConvex(const Point &p0, const Point &p1, const Point &p2, const Point &p3);

private:
	static float Det2x2(const float a00, const float a01, const float a10, const float a11);
	static float Det3x3(float A[3][3]);
	static bool Invert3x3(float A[3][3], float InvA[3][3]);
	static int MajorAxis(const Vector &v);

	static void ComputeV11BarycentricCoords(const Vector &e01, const Vector &e02, const Vector &e03, float *a11, float *b11);

	void GetUVs(float uv[4][2]) const {
		if (mesh->uvs) {
			uv[0][0] = mesh->uvs[2 * idx[0]];
			uv[0][1] = mesh->uvs[2 * idx[0] + 1];
			uv[1][0] = mesh->uvs[2 * idx[1]];
			uv[1][1] = mesh->uvs[2 * idx[1] + 1];
			uv[2][0] = mesh->uvs[2 * idx[2]];
			uv[2][1] = mesh->uvs[2 * idx[2] + 1];
			uv[3][0] = mesh->uvs[2 * idx[3]];
			uv[3][1] = mesh->uvs[2 * idx[3] + 1];
		} else {
			uv[0][0] = mesh->p[idx[0]].x;
			uv[0][1] = mesh->p[idx[0]].y;
			uv[1][0] = mesh->p[idx[1]].x;
			uv[1][1] = mesh->p[idx[1]].y;
			uv[2][0] = mesh->p[idx[2]].x;
			uv[2][1] = mesh->p[idx[2]].y;
			uv[3][0] = mesh->p[idx[3]].x;
			uv[3][1] = mesh->p[idx[3]].y;
		}
	}

	// Quadrilateral Private Data
	const Mesh *mesh;
	const int *idx;
};

}//namespace girl

