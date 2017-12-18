/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   plymesh.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - plymesh.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// plymesh.cpp*
#include "shape.h"
#include "paramset.h"

namespace girl
{

// PlyMesh Declarations
class GIRLCOREDLL  PlyMesh : public Shape {
public:
	// PlyMesh Public Methods
	PlyMesh(const Transform &o2w, bool ro, string filename, bool smooth);
	~PlyMesh();
	BBox ObjectBound() const;
	BBox WorldBound() const;
	bool CanIntersect() const { return false; }
	void Refine(vector<boost::shared_ptr<Primitive> > &refined,
			const PrimitiveRefinementHints& refineHints,
			boost::shared_ptr<Primitive> thisPtr);
	friend class Triangle;
	template <class T> friend class VertexTexture;

	static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);
public:
	// PlyMesh Data
	int ntris, nverts;
	int *vertexIndex;
	Point *p;
	Normal *n;
	Vector *s;
	float *uvs;
	vector<boost::shared_ptr<Shape> > triPtrs;
};

}//namespace girl

