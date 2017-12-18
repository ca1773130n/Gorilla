/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   bvhaccel.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - bvhaccel.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunications Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// bvhaccel.h*
#include "girl.h"
#include "primitive.h"

namespace girl
{

struct GIRLCOREDLL BVHAccelTreeNode {
	BBox bbox;
	Primitive* primitive;
	boost::shared_ptr<BVHAccelTreeNode> leftChild;
	boost::shared_ptr<BVHAccelTreeNode> rightSibling;
};

struct GIRLCOREDLL BVHAccelArrayNode {
	BBox bbox;
	Primitive* primitive;
	u_int skipIndex;
};

// BVHAccel Declarations
class GIRLCOREDLL BVHAccel : public Aggregate {
public:
	// BVHAccel Public Methods
	BVHAccel(const vector<boost::shared_ptr<Primitive> > &p, int treetype, int csamples, int icost, int tcost, float ebonus);
	~BVHAccel();
	BBox WorldBound() const;
	bool CanIntersect() const { return true; }
	bool Intersect(const Ray &ray, Intersection *isect) const;
	bool IntersectP(const Ray &ray) const;

	void GetPrimitives(vector<boost::shared_ptr<Primitive> > &prims);

	static Aggregate *CreateAccelerator(const vector<boost::shared_ptr<Primitive> > &prims, const ParamSet &ps);

private:
	// BVHAccel Private Methods
	boost::shared_ptr<BVHAccelTreeNode> BuildHierarchy(vector<boost::shared_ptr<BVHAccelTreeNode> > &list, u_int begin, u_int end, u_int axis);
	void FindBestSplit(vector<boost::shared_ptr<BVHAccelTreeNode> > &list, u_int begin, u_int end, u_int *bestIndex, u_int *bestAxis);
	u_int BuildArray(boost::shared_ptr<BVHAccelTreeNode> node, u_int offset);

	// BVHAccel Private Data
	u_char treeType;
	int costSamples, isectCost, traversalCost;
	float emptyBonus;
	u_int nPrims;
	boost::shared_ptr<Primitive> *prims;
	u_int nNodes;
	BVHAccelArrayNode *bvhTree;
};

}//namespace girl

