/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   qbvhaccel.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2009-07-07  :   First version - qbvhaccel.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2009 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// qbvhaccel.h*

#include "girl.h"
#include "primitive.h"

#include <xmmintrin.h>
#include <boost/cstdint.hpp>
using boost::int32_t;

namespace girl
{

// This code is based on Flexray by Anthony Pajot (anthony.pajot@alumni.enseeiht.fr)

/**
   QBVH accelerator, using the EGSR08 paper as base.
   need SSE !
*/

/**
   the number of bins for construction
*/
#define NB_BINS 8

/**
   The QBVH node structure, 128 bytes long (perfect for cache)
*/
class QBVHNode {
public:	
	// The constant used to represent empty leaves. there would have been
	// a conflict with a normal leaf if there were 16 quads,
	// starting at 2^27 in the quads array... very improbable.
	// using MININT (0x80000000) can produce conflict when initializing a
	// QBVH with less than 4 vertices at the beginning :
	// the number of quads - 1 would give 0, and it would start at 0
	// in the quads array
	static const int32_t emptyLeafNode = 0xffffffff;
	
	/**
	   The 4 bounding boxes, in SoA form, for direct SIMD use
	   (one __m128 for each coordinate)
	*/
	__m128 bboxes[2][3];

	/**
	   The 4 children. If a child is a leaf, its index will be negative,
	   the 4 next bits will code the number of primitives in the leaf
	   (more exactly, nbPrimitives = 4 * (p + 1), where p is the integer
	   interpretation of the 4 bits), and the 27 remaining bits the index
	   of the first quad of the node
	*/
	int32_t children[4];

	/**
	   The 3 axis of split (main, sub-left, sub-right). 0 = x, 1 = y, 2 = z
	*/
	int32_t axisMain, axisSubLeft, axisSubRight;

	/** Optimisation for shadow rays : keep the parent node index */
	int32_t parentNodeIndex;
	
	/**
	   Base constructor, init correct bounding boxes and a "root" node
	   (parentNodeIndex == -1)
	*/
	inline QBVHNode() : axisMain(0), axisSubLeft(0), axisSubRight(0),
		parentNodeIndex(-1) {
		for (int i = 0; i < 3; ++i) {
			bboxes[0][i] = _mm_set1_ps(INFINITY);
			bboxes[1][i] = _mm_set1_ps(-INFINITY);
		}
		
		// All children are empty leaves by default
		for (int i = 0; i < 4; ++i)
			children[i] = emptyLeafNode;
	}

	/**
	   Indicate whether the ith child is a leaf.
	   @param i
	   @return
	*/
	inline bool ChildIsLeaf(int i) const {
		return (children[i] < 0);
	}

	/**
	   Same thing, directly from the index.
	   @param index
	*/
	inline static bool IsLeaf(int32_t index) {
		return (index < 0);
	}

	/**
	   Indicates whether the ith child is an empty leaf.
	   @param i
	*/
	inline bool LeafIsEmpty(int i) const {
		return (children[i] == emptyLeafNode);
	}

	/**
	   Same thing, directly from the index.
	   @param index
	*/
	inline static bool IsEmpty(int32_t index) {
		return (index == emptyLeafNode);
	}
	
	/**
	   Indicate the number of quads in the ith child, which must be
	   a leaf.
	   @param i
	   @return
	*/
	inline u_int NbQuadsInLeaf(int i) const {
		return (((children[i] >> 27) & 15)) + 1;
	}

	/**
	   Return the number of group of 4 primitives, directly from the index.
	   @param index
	*/
	inline static u_int NbQuadPrimitives(int32_t index) {
		return ((index >> 27) & 15) + 1;
	}
	
	/**
	   Indicate the number of primitives in the ith child, which must be
	   a leaf.
	   @param i
	   @return
	*/
	inline u_int NbPrimitivesInLeaf(int i) const {
		return NbQuadsInLeaf(i) * 4;
	}

	/**
	   Indicate the index in the quads array of the first quad contained
	   by the the ith child, which must be a leaf.
	   @param i
	   @return
	*/
	inline u_int FirstQuadIndexForLeaf(int i) const {
		return children[i] & 0x07ffffff;
	}
	
	/**
	   Same thing, directly from the index.
	   @param index
	*/
	inline static u_int FirstQuadIndex(int32_t index) {
		return index & 0x07ffffff;
	}

	/**
	   Initialize the ith child as a leaf
	   @param i
 	   @param nbQuads
	   @param firstQuadIndex
	*/
	inline void InitializeLeaf(int i, u_int nbQuads, u_int firstQuadIndex) {
		// Take care to make a valid initialisation of the leaf.
		if (nbQuads == 0) {
			children[i] = emptyLeafNode;
		} else {
			// Put the negative sign in a plateform independent way
			children[i] = 0x80000000;//-1L & ~(-1L >> 1L);
			
			children[i] |=  ((nbQuads - 1) & 0xff) << 27;

			children[i] |= firstQuadIndex & 0x07ffffff;
		}
	}

	/**
	   Set the bounding box for the ith child.
	   @param i
	   @param bbox
	*/
	inline void SetBBox(int i, const BBox &bbox) {
		for (int axis = 0; axis < 3; ++axis) {
			((float *)&(bboxes[0][axis]))[i] = bbox.pMin[axis];
			((float *)&(bboxes[1][axis]))[i] = bbox.pMax[axis];
		}
	}

	
	/**
	   Intersect a ray described by sse variables with the 4 bounding boxes
	   of the node.
	   @param sseOrig the 3 coordinates replicated on a SSE register
	   @param sseInvDir the 3 coordinates of the inverse of the direction
	   @param sseTMin
	   @param sseTMax
	   @param sign
	   @param bboxOrder will contain the order in which the childrn have
	   to be intersected after
	   @return an int used to index the array of paths in the bboxes
	   (the visit array)
	*/
	int32_t BBoxIntersect(__m128 sseOrig[3], __m128 sseInvDir[3],
		const __m128 &sseTMin, const __m128 &sseTMax,
		const int sign[3]) const;

	
};


/***************************************************/
class QBVHAccel : public Aggregate {
public:
	/**
	   Normal constructor.
	   @param p the vector of shared primitives to put in the QBVH
	   @param mp the maximum number of primitives per leaf
	   @param fst the threshold before switching to full sweep for split
	   @param sf the skip factor during split determination
	*/
	QBVHAccel(const vector<boost::shared_ptr<Primitive> > &p, int mp, float fst, int sf);

	/**
	   to free the memory.
	*/
	virtual ~QBVHAccel();
	
	/**
	   to get the world bbox.
	   @return
	*/
	BBox WorldBound() const;   

	/**
	   Intersect a ray in world space against the
	   primitive and fills in an Intersection object.
	   @param ray in world space
	   @param isect pointer to the intersection object to fill.
	   @return true if there is an intersection.
	*/
	bool Intersect(const Ray &ray, Intersection *isect) const;

	/**
	   Predicate version, only tests if there is intersection.
	   @param ray in world space
	   @return true if there is intersection.
	*/
	bool IntersectP(const Ray &ray) const;

	/**
	   Fills an array with the primitives
	   @param prims vector to be filled
	*/
	void GetPrimitives(vector<boost::shared_ptr<Primitive> > &prims);

	/**
	   Read configuration parameters and create a new QBVH accelerator
	   @param prims vector of primitives to store into the QBVH
	   @param ps configuration parameters
	*/
	static Aggregate *CreateAccelerator(const vector<boost::shared_ptr<Primitive> > &prims, const ParamSet &ps);

private:
	/**
	   Build the tree that will contain the primitives indexed from start
	   to end in the primsIndexes array.
	   @param start
	   @param end
	   @param primsBboxes the bounding boxes for all the primitives
	   @param primsbboxes the centroids of all the primitives
	   @param nodeBbox the bounding box of the node.
	   @param centroidsBbox the bounding box of the centroids of the
	   primitives in the node.
	   @param parentIndex the index of the parent node
	   @param childIndex the index of the node in the parent node
	   (its child number)
	   @param depth the current depth.
	*/
	void BuildTree(u_int start, u_int end, u_int *primsIndexes, BBox *primsBboxes,
		Point *primsCentroids, const BBox &nodeBbox,
		const BBox &centroidsBbox, int32_t parentIndex, int32_t childIndex,
		int depth);
	
	/**
	   Create a leaf using the traditional QBVH layout
	   @param parentIndex
	   @param childIndex
	   @param start
	   @param end
	   @param nodeBbox
	*/
	void CreateTempLeaf(int32_t parentIndex, int32_t childIndex, u_int start, u_int end,
		const BBox &nodeBbox);

	/**
	   Create an intermediate node
	   @param parentIndex
	   @param childIndex
	   @param nodeBbox
	*/
	inline int32_t CreateIntermediateNode(int32_t parentIndex, int32_t childIndex,
		const BBox &nodeBbox) {
		int32_t index = nNodes++; // increment after assignment
		nodes[index].parentNodeIndex = parentIndex;
		if (parentIndex >= 0) {
			nodes[parentIndex].children[childIndex] = index;
			nodes[parentIndex].SetBBox(childIndex, nodeBbox);
		}
		return index;
	}

	/**
	   switch a node and its subnodes from the
	   traditional form of QBVH to the pre-swizzled one.
	   @param nodeIndex
	   @param primsIndexes
	   @param vPrims
	*/
	void PreSwizzle(int32_t nodeIndex, u_int *primsIndexes,
		const vector<boost::shared_ptr<Primitive> > &vPrims);

	/**
	   Create a leaf using the pre-swizzled layout,
	   using the informations stored in the node that
	   are organized following the traditional layout
	   @param parentIndex
	   @param childIndex
	   @param primsIndexes
	   @param vPrims
	*/
	void CreateSwizzledLeaf(int32_t parentIndex, int32_t childIndex, 
		u_int *primsIndexes, const vector<boost::shared_ptr<Primitive> > &vPrims);

	/**
	   the actual number of quads
	*/
	u_int nQuads;

	/**
	   The primitive associated with each triangle. indexed by the number of quad
	   and the number of triangle in the quad (thus, there might be holes).
	   no need to be a tesselated primitive, the intersection
	   test will be redone for the nearest triangle found, to
	   fill the Intersection structure.
	*/
	boost::shared_ptr<Primitive> *prims;
	
	/**
	   The number of primitives
	*/
	u_int nPrims;

	/**
	   The nodes of the QBVH.
	*/
	QBVHNode *nodes;

	/**
	   The number of nodes really used.
	*/
	int32_t nNodes;

	/**
	   The world bounding box of the QBVH.
	*/
	BBox worldBound;

	/**
	   The number of primitives in the node that makes switch
	   to full sweep for binning
	*/
	u_int fullSweepThreshold;

	/**
	   The skip factor for binning
	*/
	u_int skipFactor;

	/**
	   The maximum number of primitives per leaf
	*/
	u_int maxPrimsPerLeaf;

	
	// Adapted from Robin Bourianes (robin.bourianes@free.fr)
	// Array indicating the order of visit

	// This table needs to be indexed like this :
	// const u_int idx = (((r.d.Signs()[node->axisMain]) << 2) |
	//                   ((r.d.Signs()[node->axisSubLeft]) << 1) |
	//                   ((r.d.Signs()[node->axisSubRight])));
	// const u_int visit = bbox[0].IntersectP(ray) |
	//                     bbox[1].IntersectP(ray) << 1 |
	//                     bbox[2].IntersectP(ray) << 2 |
	//                     bbox[3].IntersectP(ray) << 3;
	// const u_int finalIdx = visit * 32 + idx * 4;
	// It contains bounding box indices sorted by distance from the ray
	// origin.
	// 4 bits per index, stored in 32bit int. 4 means no intersection.
	// 16 visit * 8 idx * 4 bbox = 128 * ints = 512bytes
	static const boost::int16_t pathTable[128];
};

} // namespace girl
