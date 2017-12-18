/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   grid.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - grid.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////


// grid.cpp*

#include "girl.h"
#include "primitive.h"
#include <boost/pool/object_pool.hpp>

namespace girl
{

// GridAccel Forward Declarations
struct GMailboxPrim;
struct Voxel;
// GMailboxPrim Declarations

struct GMailboxPrim {
	GMailboxPrim(boost::shared_ptr<Primitive> p) {
		primitive = p;
		lastMailboxId = -1;
	}
	boost::shared_ptr<Primitive> primitive;
	int lastMailboxId;
};


// Voxel Declarations
struct Voxel {
	// Voxel Public Methods
	Voxel(GMailboxPrim *op) {
		allCanIntersect = false;
		nPrimitives = 1;
		onePrimitive = op;
	}
	void AddPrimitive(GMailboxPrim *prim) {
		if (nPrimitives == 1) {
			// Allocate initial _primitives_ array in voxel
			GMailboxPrim **p = new GMailboxPrim *[2];
			p[0] = onePrimitive;
			primitives = p;
		}
		else if (IsPowerOf2(nPrimitives)) {
			// Increase size of _primitives_ array in voxel
			int nAlloc = 2 * nPrimitives;
			GMailboxPrim **p = new GMailboxPrim *[nAlloc];
			for (u_int i = 0; i < nPrimitives; ++i)
				p[i] = primitives[i];
			delete[] primitives;
			primitives = p;
		}
		primitives[nPrimitives] = prim;
		++nPrimitives;
	}
	~Voxel() {
		if (nPrimitives > 1) delete[] primitives;
	}
	bool Intersect(const Ray &ray,
	               Intersection *isect,
				   int rayId);
	bool IntersectP(const Ray &ray, int rayId);
	union {
		GMailboxPrim *onePrimitive;
		GMailboxPrim **primitives;
	};
	u_int allCanIntersect:1;
	u_int nPrimitives:31;
};
// GridAccel Declarations
class GIRLCOREDLL   GridAccel : public Aggregate {
public:
	// GridAccel Public Methods
	GridAccel(const vector<boost::shared_ptr<Primitive> > &p,
	          bool forRefined, bool refineImmediately);
	BBox WorldBound() const;
	bool CanIntersect() const { return true; }
	~GridAccel();
	bool Intersect(const Ray &ray, Intersection *isect) const;
	bool IntersectP(const Ray &ray) const;

	void GetPrimitives(vector<boost::shared_ptr<Primitive> > &prims);

	static Aggregate *CreateAccelerator(const vector<boost::shared_ptr<Primitive> > &prims, const ParamSet &ps);
private:
	// GridAccel Private Methods
	int PosToVoxel(const Point &P, int axis) const {
		int v = Float2Int((P[axis] - bounds.pMin[axis]) *
		                  InvWidth[axis]);
		return Clamp(v, 0, NVoxels[axis]-1);
	}
	float VoxelToPos(int p, int axis) const {
		return bounds.pMin[axis] + p * Width[axis];
	}
	Point VoxelToPos(int x, int y, int z) const {
		return bounds.pMin +
			Vector(x * Width[0], y * Width[1], z * Width[2]);
	}
	inline int Offset(int x, int y, int z) const {
		return z*NVoxels[0]*NVoxels[1] + y*NVoxels[0] + x;
	}
	// GridAccel Private Data
	bool gridForRefined;
	u_int nMailboxes;
	GMailboxPrim *mailboxes;
	int NVoxels[3];
	BBox bounds;
	Vector Width, InvWidth;
	Voxel **voxels;
	//ObjectArena<Voxel> voxelArena;
	//boost::pool<> p(sizeof(int));
	boost::object_pool<Voxel> voxelArena;
	static int curMailboxId;
};

}//namespace girl

