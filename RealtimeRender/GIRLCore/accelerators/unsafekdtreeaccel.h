/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   unsafekdtreeaccel.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - unsafekdtreeaccel.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_UNSAFEKDTREEACCEL_H
#define GIRL_UNSAFEKDTREEACCEL_H

// unsafekdtree.cpp*
#include "girl.h"
#include "primitive.h"
#include "memory.h"

namespace girl {

// UnsafeKdAccelNode Declarations

struct MailboxPrim {
    MailboxPrim(boost::shared_ptr<Primitive> p) {
        primitive = p;
        lastMailboxId = -1;
    }
    boost::shared_ptr<Primitive> primitive;
    int lastMailboxId;
};

struct UnsafeKdAccelNode {
    // UnsafeKdAccelNode Methods
    void initLeaf(int *primNums, int np,
            MailboxPrim *mailboxPrims, MemoryArena &arena) {
        // Update kd leaf node allocation statistics
        // radiance - disabled for threading // static StatsCounter numLeafMade("Kd-Tree Accelerator","Leaf kd-tree nodes made");
        // radiance - disabled for threading // static StatsCounter maxDepth("Kd-Tree Accelerator","Maximum kd-tree depth");
        // radiance - disabled for threading // static StatsCounter maxLeafPrims("Kd-Tree Accelerator","Maximum number of primitives in leaf node");
        // radiance - disabled for threading // ++numLeafMade;
        //maxDepth.Max(depth);
        // radiance - disabled for threading // maxLeafPrims.Max(np);
        // radiance - disabled for threading // static StatsRatio leafPrims("Kd-Tree Accelerator","Avg. number of primitives in leaf nodes");
        // radiance - disabled for threading // leafPrims.Add(np, 1);
        nPrims = np << 2;
        flags |= 3;
        // Store _MailboxPrim *_s for leaf node
        if (np == 0)
            onePrimitive = NULL;
        else if (np == 1)
            onePrimitive = &mailboxPrims[primNums[0]];
        else {
            primitives = (MailboxPrim **)arena.Alloc(np *
                    sizeof(MailboxPrim *));
            for (int i = 0; i < np; ++i)
                primitives[i] = &mailboxPrims[primNums[i]];
        }
    }
    void initInterior(int axis, float s) {
        // radiance - disabled for threading // static StatsCounter nodesMade("Kd-Tree Accelerator", "Interior kd-tree nodes made"); // NOBOOK
        // radiance - disabled for threading // ++nodesMade; // NOBOOK
        split = s;
        flags &= ~3;
        flags |= axis;
    }
    float SplitPos() const { return split; }
    int nPrimitives() const { return nPrims >> 2; }
    int SplitAxis() const { return flags & 3; }
    bool IsLeaf() const { return (flags & 3) == 3; }

    // NOTE - radiance - applied bugfix for light leaks on planes (found by ratow)
    // moved flags outside of union
    u_int flags;   // Both
    union {
        //u_int flags;   // Both
        float split;   // Interior
        u_int nPrims;  // Leaf
    };
    union {
        u_int aboveChild;           // Interior
        MailboxPrim *onePrimitive;  // Leaf
        MailboxPrim **primitives;   // Leaf
    };
};

struct UnsafeBoundEdge {
    // UnsafeBoundEdge Public Methods
    UnsafeBoundEdge() { }
    UnsafeBoundEdge(float tt, int pn, bool starting) {
        t = tt;
        primNum = pn;
        type = starting ? START : END;
    }
    bool operator<(const UnsafeBoundEdge &e) const {
        if (t == e.t)
            return (int)type < (int)e.type;
        else return t < e.t;
    }
    float t;
    int primNum;
    enum { START, END } type;
};

// UnsafeKdTreeAccel Declarations
struct UnsafeKdAccelNode;
class GIRLCOREDLL   UnsafeKdTreeAccel : public Aggregate {
public:
    // UnsafeKdTreeAccel Public Methods
    UnsafeKdTreeAccel(const vector<boost::shared_ptr<Primitive> > &p,
        int icost, int scost,
        float ebonus, int maxp, int maxDepth);
    BBox WorldBound() const { return bounds; }
    bool CanIntersect() const { return true; }
    ~UnsafeKdTreeAccel();
    void buildTree(int nodeNum, const BBox &bounds,
            const vector<BBox> &primBounds,
            int *primNums, int nprims, int depth,
            UnsafeBoundEdge *edges[3],
            int *prims0, int *prims1, int badRefines = 0);
    bool Intersect(const Ray &ray, Intersection *isect) const;
    bool IntersectP(const Ray &ray) const;

    void GetPrimitives(vector<boost::shared_ptr<Primitive> > &prims);

    static Aggregate *CreateAccelerator(const vector<boost::shared_ptr<Primitive> > &prims, const ParamSet &ps);

private:
    // UnsafeKdTreeAccel Private Data
    BBox bounds;
    int isectCost, traversalCost, maxPrims;
    float emptyBonus;
    u_int nMailboxes;
    MailboxPrim *mailboxPrims;
    mutable int curMailboxId;
    UnsafeKdAccelNode *nodes;
    int nAllocedNodes, nextFreeNode;

    MemoryArena arena;
};

struct KdToDo {
    const UnsafeKdAccelNode *node;
    float tmin, tmax;
};

}//namespace girl

#endif

