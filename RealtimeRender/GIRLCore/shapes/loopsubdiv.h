/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   loopsubdiv.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - loopsubdiv.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// loopsubdiv.cpp*
#include "shape.h"
#include "texture.h"
#include "error.h"
#include <set>
#include <map>
using std::set;
using std::map;
// LoopSubdiv Macros
#define NEXT(i) (((i)+1)%3)
#define PREV(i) (((i)+2)%3)

namespace girl
{

// LoopSubdiv Local Structures
struct SDFace;
struct SDFace;
struct SDVertex {
	// SDVertex Constructor
	SDVertex(Point pt = Point(0,0,0), float uu = 0.0f, float vv = 0.0f)
		: P(pt), u(uu), v(vv), startFace(NULL), child(NULL),
		regular(false), boundary(false) {
	}

	// SDVertex Methods
	int valence();
	void oneRing(Point *P);
	void oneRing(SDVertex **V);

	Point P;
	float u, v;
	SDFace *startFace;
	SDVertex *child;
	bool regular, boundary, hasUV;
};

struct SDFace {
	// SDFace Constructor
	SDFace() {
		int i;
		for (i = 0; i < 3; ++i) {
			v[i] = NULL;
			f[i] = NULL;
		}
		for (i = 0; i < 4; ++i)
			children[i] = NULL;
	}
	// SDFace Methods
	int vnum(SDVertex *vert) const {
		for (int i = 0; i < 3; ++i)
			if (v[i] == vert) return i;
		girlError(GIRL_BUG,GIRL_SEVERE,"Basic logic error in SDFace::vnum()");
		return -1;
	}
	SDFace *nextFace(SDVertex *vert) {
		return f[vnum(vert)];
	}
	SDFace *prevFace(SDVertex *vert) {
		return f[PREV(vnum(vert))];
	}
	SDVertex *nextVert(SDVertex *vert) {
		return v[NEXT(vnum(vert))];
	}
	SDVertex *prevVert(SDVertex *vert) {
		return v[PREV(vnum(vert))];
	}
	SDVertex *otherVert(SDVertex *v0, SDVertex *v1) {
		for (int i = 0; i < 3; ++i)
			if (v[i] != v0 && v[i] != v1)
				return v[i];
		girlError(GIRL_BUG,GIRL_SEVERE,"Basic logic error in SDVertex::otherVert()");
		return NULL;
	}
	SDVertex *v[3];
	SDFace *f[3];
	SDFace *children[4];
};

struct SDEdge {
	// SDEdge Constructor
	SDEdge(SDVertex *v0 = NULL, SDVertex *v1 = NULL) {
		v[0] = min(v0, v1);
		v[1] = max(v0, v1);
		f[0] = f[1] = NULL;
		f0edgeNum = -1;
	}
	// SDEdge Comparison Function
	bool operator<(const SDEdge &e2) const {
		if (v[0] == e2.v[0]) return v[1] < e2.v[1];
		return v[0] < e2.v[0];
	}
	SDVertex *v[2];
	SDFace *f[2];
	int f0edgeNum;
};

// LoopSubdiv Declarations
class GIRLCOREDLL  LoopSubdiv : public Shape {
public:
	// LoopSubdiv Public Methods
	LoopSubdiv(const Transform &o2w, bool ro,
			int nt, int nv, const int *vi,
			const Point *P, const float *uv, int nlevels,
			const boost::shared_ptr<Texture<float> > dismap,
			float dmscale, float dmoffset,
			bool dmnormalsmooth, bool dmsharpboundary);
	~LoopSubdiv();
	bool CanIntersect() const;
	void Refine(vector<boost::shared_ptr<Shape> > &refined) const;
	BBox ObjectBound() const;
	BBox WorldBound() const;

	static Shape *CreateShape(const Transform &o2w, bool reverseOrientation,
			const ParamSet &params);

	class SubdivResult {
	public:
		SubdivResult(int aNtris, int aNverts, const int* aIndices,
			const Point *aP, const Normal *aN, const float *aUv)
			: ntris(aNtris), nverts(aNverts), indices(aIndices),
			P(aP), N(aN), uv(aUv)
		{
		}
		~SubdivResult() {
			delete[] indices;
			delete[] P;
			delete[] N;
			if( uv )
				delete[] uv;
		}

		const int ntris;
		const int nverts;

		const int * const indices;
		const Point * const P;
		const Normal * const N;
		const float * const uv;
	};
	boost::shared_ptr<SubdivResult> Refine() const;

private:
	// LoopSubdiv Private Methods
	float beta(int valence) const {
		if (valence == 3) return 3.f/16.f;
		else return 3.f / (8.f * valence);
	}
	void weightOneRing(SDVertex *destVert, SDVertex *vert, float beta) const ;
	void weightBoundary(SDVertex *destVert, SDVertex *vert, float beta) const;
	float gamma(int valence) const {
		return 1.f / (valence + 3.f / (8.f * beta(valence)));
	}
	static void GenerateNormals(const vector<SDVertex *> verts, Normal *Ns);

	void ApplyDisplacementMap(
			const vector<SDVertex *> verts,
			const Normal *norms,
			const float *uvs) const;

	// LoopSubdiv Private Data
	int nLevels;
	vector<SDVertex *> vertices;
	vector<SDFace *> faces;

	// Dade - optional displacement map
	boost::shared_ptr<Texture<float> > displacementMap;
	float displacementMapScale;
	float displacementMapOffset;

	bool hasUV, displacementMapNormalSmooth, displacementMapSharpBoundary;

	// Lotus - a pointer to the refined mesh to avoid double refinement or deletion
	mutable boost::shared_ptr<Shape> refinedShape;
};

// LoopSubdiv Inline Functions
inline int SDVertex::valence() {
	SDFace *f = startFace;
	if (!boundary) {
		// Compute valence of interior vertex
		int nf = 1;
		while ((f = f->nextFace(this)) != startFace)
			++nf;
		return nf;
	}
	else {
		// Compute valence of boundary vertex
		int nf = 1;
		while ((f = f->nextFace(this)) != NULL)
			++nf;
		f = startFace;
		while ((f = f->prevFace(this)) != NULL)
			++nf;
		return nf+1;
	}
}

}//namespace girl

