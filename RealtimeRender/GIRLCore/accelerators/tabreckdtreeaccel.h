/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   tabreckdtreeaccel.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - tabreckdtreeaccel.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_TABRECKDTREEACCEL_H
#define GIRL_TABRECKDTREEACCEL_H

// tabreckdtree.cpp*
#include "girl.h"
#include "primitive.h"
#include "memory.h"

namespace girl
{

struct TaBRecKdAccelNode {
    // TaBRecKdAccelNode Methods
    void initLeaf(int *primNums, int np,
            boost::shared_ptr<Primitive> *prims, MemoryArena &arena) {
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
        // Store _Primitive *_s for leaf node
        if (np == 0)
            onePrimitive = NULL;
        else if (np == 1)
            onePrimitive = prims[primNums[0]].get();
        else {
            primitives = (Primitive **)arena.Alloc(np *
                    sizeof(Primitive **));
            for (int i = 0; i < np; ++i)
                primitives[i] = prims[primNums[i]].get();
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
    //u_int flags;   // Both
    // Dade - placing flags inside the union was intended by PBRT authors. It is
    // used at the same time of split or nPrims in order to set the lower 2 bits
    // of the long word. Read the PBRT book (kdtree chapter, about at pag. 200)
    // for more details. If there is a light leak in some case, another kind
    // of fix must be find.

    union {
        u_int flags;   // Both
        float split;   // Interior
        u_int nPrims;  // Leaf
    };
    union {
        u_int aboveChild;         // Interior
        Primitive *onePrimitive;  // Leaf
        Primitive **primitives;   // Leaf
    };
};

struct TaBRecBoundEdge {
    // TaBRecBoundEdge Public Methods
    TaBRecBoundEdge() { }
    TaBRecBoundEdge(float tt, int pn, bool starting) {
        t = tt;
        primNum = pn;
        type = starting ? START : END;
    }
    bool operator<(const TaBRecBoundEdge &e) const {
        if (t == e.t)
            return (int)type < (int)e.type;
        else return t < e.t;
    }
    float t;
    int primNum;
    enum { START, END } type;
};

// Dade - inverse mailbox support. I use a ring buffer in order to
// store a list of already intersected primitives.

// Dade - implementation with hardcoded size
struct TaBRecInverseMailboxes {
    int indexFirstFree;
    Primitive *mailboxes[8];

    TaBRecInverseMailboxes() {
        indexFirstFree = 0;

        Primitive** mb = mailboxes;
        *mb++ = NULL; // mailboxes[0]
        *mb++ = NULL; // mailboxes[1]
        *mb++ = NULL; // mailboxes[2]
        *mb++ = NULL; // mailboxes[3]
        *mb++ = NULL; // mailboxes[4]
        *mb++ = NULL; // mailboxes[5]
        *mb++ = NULL; // mailboxes[6]
        *mb = NULL; // mailboxes[7]
    }

    void addChecked(Primitive *p) {
        mailboxes[indexFirstFree++] = p;
        indexFirstFree &= 0x7;
    }

    bool alreadyChecked(const Primitive *p) const {
        Primitive* const* mb = mailboxes;

        if (*mb++ == p) // mailboxes[0]
            return true;
        if (*mb++ == p) // mailboxes[1]
            return true;
        if (*mb++ == p) // mailboxes[2]
            return true;
        if (*mb++ == p) // mailboxes[3]
            return true;
        if (*mb++ == p) // mailboxes[4]
            return true;
        if (*mb++ == p) // mailboxes[5]
            return true;
        if (*mb++ == p) // mailboxes[6]
            return true;
        if (*mb == p)   // mailboxes[7]
            return true;

        return false;
    }
};

// TaBRecKdTreeAccel Declarations
class GIRLCOREDLL   TaBRecKdTreeAccel : public Aggregate {
public:
    // TaBRecKdTreeAccel Public Methods
	TaBRecKdTreeAccel(){};
    TaBRecKdTreeAccel(const vector<boost::shared_ptr<Primitive> > &p,
            int icost, int scost,
            float ebonus, int maxp, int maxDepth);
    BBox WorldBound() const { return bounds; }
    bool CanIntersect() const { return true; }
    ~TaBRecKdTreeAccel();
    void buildTree(int nodeNum, const BBox &bounds,
            const vector<BBox> &primBounds,
            int *primNums, int nprims, int depth,
            TaBRecBoundEdge *edges[3],
            int *prims0, int *prims1, int badRefines = 0);
    bool Intersect(const Ray &ray, Intersection *isect) const;
    bool IntersectP(const Ray &ray) const;

    void GetPrimitives(vector<boost::shared_ptr<Primitive> > &prims);

    static Aggregate *CreateAccelerator(const vector<boost::shared_ptr<Primitive> > &prims, const ParamSet &ps);

//private:
protected:
    // TaBRecKdTreeAccel Private Data
    BBox bounds;
    int isectCost, traversalCost, maxPrims;
    float emptyBonus;

    u_int nPrims;
    boost::shared_ptr<Primitive> *prims;
    TaBRecKdAccelNode *nodes;
    int nAllocedNodes, nextFreeNode;

    MemoryArena arena;
};

struct TaBRecKdNodeStack {
    const TaBRecKdAccelNode *node;
    float t;
    // Dade - Entry/exit point
    Point pb;
    // Dade - index of the previous stack element
    int prev;
};

}//namespace girl

#endif

