/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   tabreckdtree.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - tabreckdtree.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// tabreckdtree.cpp*
#include "tabreckdtreeaccel.h"
#include "paramset.h"
#include "error.h"
#include "dynload.h"

using namespace girl;

// TaBRecKdTreeAccel Method Definitions
TaBRecKdTreeAccel::
TaBRecKdTreeAccel(const vector<boost::shared_ptr<Primitive> > &p,
        int icost, int tcost,
        float ebonus, int maxp, int maxDepth)
: isectCost(icost), traversalCost(tcost),
        maxPrims(maxp), emptyBonus(ebonus) {
    vector<boost::shared_ptr<Primitive> > vPrims;
    const PrimitiveRefinementHints refineHints(false);
    for (u_int i = 0; i < p.size(); ++i) {
    	if(p[i]->CanIntersect())
    		vPrims.push_back(p[i]);
    	else
    		p[i]->Refine(vPrims, refineHints, p[i]);
    }

    // Initialize primitives for _TaBRecKdTreeAccel_
    nPrims = vPrims.size();
    prims = (boost::shared_ptr<Primitive>*)AllocAligned(nPrims *
    		sizeof(boost::shared_ptr<Primitive>));
    for (u_int i = 0; i < nPrims; ++i)
    	new (&prims[i]) boost::shared_ptr<Primitive>(vPrims[i]);

    // Build kd-tree for accelerator
    nextFreeNode = nAllocedNodes = 0;
    if (maxDepth <= 0)
        maxDepth =
                Round2Int(8 + 1.3f * Log2Int(float(vPrims.size())));

    // Compute bounds for kd-tree construction
    vector<BBox> primBounds;
    primBounds.reserve(vPrims.size());
    for (u_int i = 0; i < vPrims.size(); ++i) {
        BBox b = prims[i]->WorldBound();

        // Dade - expand the bbox by EPSILON in order to avoid numerical problems
        Vector bbEdge = b.pMax - b.pMin;
        if (bbEdge.x < 2.0f * RAY_EPSILON) {
            b.pMin.x -= RAY_EPSILON;
            b.pMax.x += RAY_EPSILON;
        }
        if (bbEdge.y < 2.0f * RAY_EPSILON) {
            b.pMin.y -= RAY_EPSILON;
            b.pMax.y += RAY_EPSILON;
        }
        if (bbEdge.z < 2.0f * RAY_EPSILON) {
            b.pMin.z -= RAY_EPSILON;
            b.pMax.z += RAY_EPSILON;
        }

        bounds = Union(bounds, b);
        primBounds.push_back(b);
    }

    // Allocate working memory for kd-tree construction
    TaBRecBoundEdge *edges[3];
    for (int i = 0; i < 3; ++i)
        edges[i] = new TaBRecBoundEdge[2*vPrims.size()];
    int *prims0 = new int[vPrims.size()];
    int *prims1 = new int[(maxDepth+1) * vPrims.size()];
    // Initialize _primNums_ for kd-tree construction
    int *primNums = new int[vPrims.size()];
    for (u_int i = 0; i < vPrims.size(); ++i)
        primNums[i] = i;

	std::stringstream ss;
	ss << "Building KDTree, primitives: " << nPrims;
	//girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());
	girlProgressMsgPrint(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

    // Start recursive construction of kd-tree
    buildTree(0, bounds, primBounds, primNums,
            vPrims.size(), maxDepth, edges,
            prims0, prims1);
    // Free working memory for kd-tree construction
    delete[] primNums;
    for (int i = 0; i < 3; ++i)
        delete[] edges[i];
    delete[] prims0;
    delete[] prims1;
}

TaBRecKdTreeAccel::~TaBRecKdTreeAccel() {
	for(u_int i=0; i<nPrims; i++)
		prims[i].~shared_ptr();
    FreeAligned(prims);
    FreeAligned(nodes);
}

void TaBRecKdTreeAccel::buildTree(int nodeNum,
        const BBox &nodeBounds,
        const vector<BBox> &allPrimBounds, int *primNums,
        int nPrims, int depth, TaBRecBoundEdge *edges[3],
        int *prims0, int *prims1, int badRefines) {
    BOOST_ASSERT(nodeNum == nextFreeNode); // NOBOOK
    // Get next free node from _nodes_ array
    if (nextFreeNode == nAllocedNodes) {
        int nAlloc = max(2 * nAllocedNodes, 512);
        TaBRecKdAccelNode *n = (TaBRecKdAccelNode *)AllocAligned(nAlloc *
                sizeof(TaBRecKdAccelNode));
        if (nAllocedNodes > 0) {
            memcpy(n, nodes,
                    nAllocedNodes * sizeof(TaBRecKdAccelNode));
            FreeAligned(nodes);
        }
        nodes = n;
        nAllocedNodes = nAlloc;
    }
    ++nextFreeNode;
    // Initialize leaf node if termination criteria met
    if (nPrims <= maxPrims || depth == 0) {
        nodes[nodeNum].initLeaf(primNums, nPrims, prims, arena);
        return;
    }
    // Initialize interior node and continue recursion
    // Choose split axis position for interior node
    int bestAxis = -1, bestOffset = -1;
    float bestCost = INFINITY;
    float oldCost = isectCost * float(nPrims);
    Vector d = nodeBounds.pMax - nodeBounds.pMin;
    float totalSA = (2.f * (d.x*d.y + d.x*d.z + d.y*d.z));
    float invTotalSA = 1.f / totalSA;
    // Choose which axis to split along
    int axis;
    if (d.x > d.y && d.x > d.z) axis = 0;
    else axis = (d.y > d.z) ? 1 : 2;
    int retries = 0;
    retrySplit:
        // Initialize edges for _axis_
        for (int i = 0; i < nPrims; ++i) {
            int pn = primNums[i];
            const BBox &bbox = allPrimBounds[pn];
            edges[axis][2*i] =
                    TaBRecBoundEdge(bbox.pMin[axis], pn, true);
            edges[axis][2*i+1] =
                    TaBRecBoundEdge(bbox.pMax[axis], pn, false);
        }
    sort(&edges[axis][0], &edges[axis][2*nPrims]);
    // Compute cost of all splits for _axis_ to find best
    int nBelow = 0, nAbove = nPrims;
    for (int i = 0; i < 2*nPrims; ++i) {
        if (edges[axis][i].type == TaBRecBoundEdge::END) --nAbove;
        float edget = edges[axis][i].t;
        if (edget > nodeBounds.pMin[axis] &&
        edget < nodeBounds.pMax[axis]) {
            // Compute cost for split at _i_th edge
            int otherAxis[3][2] = { {1, 2}, {0, 2}, {0, 1} };
            int otherAxis0 = otherAxis[axis][0];
            int otherAxis1 = otherAxis[axis][1];
            float belowSA = 2 * (d[otherAxis0] * d[otherAxis1] +
            (edget - nodeBounds.pMin[axis]) *
            (d[otherAxis0] + d[otherAxis1]));
            float aboveSA = 2 * (d[otherAxis0] * d[otherAxis1] +
            (nodeBounds.pMax[axis] - edget) *
            (d[otherAxis0] + d[otherAxis1]));
            float pBelow = belowSA * invTotalSA;
            float pAbove = aboveSA * invTotalSA;
            float eb = (nAbove == 0 || nBelow == 0) ? emptyBonus : 0.f;
            float cost = traversalCost + isectCost * (1.f - eb) *
            (pBelow * nBelow + pAbove * nAbove);
            // Update best split if this is lowest cost so far
            if (cost < bestCost)  {
                bestCost = cost;
                bestAxis = axis;
                bestOffset = i;
            }
        }
        if (edges[axis][i].type == TaBRecBoundEdge::START) ++nBelow;
    }
    BOOST_ASSERT(nBelow == nPrims && nAbove == 0); // NOBOOK
    // Create leaf if no good splits were found
    if (bestAxis == -1 && retries < 2) {
        ++retries;
        axis = (axis+1) % 3;
        goto retrySplit;
    }
    if (bestCost > oldCost) ++badRefines;
    if ((bestCost > 4.f * oldCost && nPrims < 16) ||
            bestAxis == -1 || badRefines == 3) {
        nodes[nodeNum].initLeaf(primNums, nPrims, prims, arena);
        return;
    }
    // Classify primitives with respect to split
    int n0 = 0, n1 = 0;
    for (int i = 0; i < bestOffset; ++i)
        if (edges[bestAxis][i].type == TaBRecBoundEdge::START)
            prims0[n0++] = edges[bestAxis][i].primNum;
    for (int i = bestOffset+1; i < 2*nPrims; ++i)
        if (edges[bestAxis][i].type == TaBRecBoundEdge::END)
            prims1[n1++] = edges[bestAxis][i].primNum;
    // Recursively initialize children nodes
    float tsplit = edges[bestAxis][bestOffset].t;
    nodes[nodeNum].initInterior(bestAxis, tsplit);
    BBox bounds0 = nodeBounds, bounds1 = nodeBounds;
    bounds0.pMax[bestAxis] = bounds1.pMin[bestAxis] = tsplit;
    buildTree(nodeNum+1, bounds0,
            allPrimBounds, prims0, n0, depth-1, edges,
            prims0, prims1 + nPrims, badRefines);
    nodes[nodeNum].aboveChild = nextFreeNode;
    buildTree(nodes[nodeNum].aboveChild, bounds1, allPrimBounds,
            prims1, n1, depth-1, edges,
            prims0, prims1 + nPrims, badRefines);
}

// Dade - this code is based on Appendix C of Ph.D. Thesis by Vlastimil Havran
// "Heuristic Ray Shooting Algorithms" available at http://www.cgg.cvut.cz/members/havran/phdthesis.html
// TaBRecKdTreeAccel::Intersect uses limts in mint/maxt while TaBRecKdTreeAccel::IntersectP
// uses inverse mailboxes, it looks like the fastest combo.
bool TaBRecKdTreeAccel::Intersect(const Ray &ray,
        Intersection *isect) const {
    // Compute initial parametric range of ray inside kd-tree extent
    float t, tmin, tmax;
    if (!bounds.IntersectP(ray, &tmin, &tmax))
        return false;

	const float originalMint = ray.mint;
    const float originalMaxt = ray.maxt;

    // Prepare to traverse kd-tree for ray
    Vector invDir(1.f/ray.d.x, 1.f/ray.d.y, 1.f/ray.d.z);
    #define MAX_TODO 64
    TaBRecKdNodeStack stack[MAX_TODO];
    int enPt = 0;
    stack[enPt].t = tmin;

    // Distinguish between internal and external origin
    if (tmin >= 0.0f)
        stack[enPt].pb = ray.o + ray.d * tmin;
    else
        stack[enPt].pb = ray.o;

    // Setup initial exit point in the stack
    int exPt = 1; // Pointer to the stack
    stack[exPt].t = tmax;
    stack[exPt].pb = ray.o + ray.d * tmax;
    stack[exPt].node = NULL; // Set termination flag

    const TaBRecKdAccelNode *currNode = &nodes[0];
    const TaBRecKdAccelNode *farChild;
    while (currNode != NULL) {
        while (!currNode->IsLeaf()) {
            // Retrieve position of splitting plane
            float splitVal = currNode->SplitPos();
            int axis = currNode->SplitAxis();

            if (stack[enPt].pb[axis] <= splitVal) {
                if (stack[exPt].pb[axis] <= splitVal) {
                    // Case N1, N2, N3, P5, Z2, and Z3
                    currNode = currNode + 1;
                    continue;
                }
                if (stack[enPt].pb[axis] == splitVal) {
                    currNode = &nodes[currNode->aboveChild];
                    continue; // case Z1
                }

                // Case N4

                farChild = &nodes[currNode->aboveChild];
                currNode = currNode + 1;
            } else {
                if (splitVal < stack[exPt].pb[axis]) {
                    // Case P1, P2, P3, and N5

                    currNode = &nodes[currNode->aboveChild];
                    continue;
                }

                // Case P4
                farChild = currNode + 1;
                currNode = &nodes[currNode->aboveChild];
            }

            // Case P4 or N4 . . . traverse both children

            // Signed distance to the splitting plane
            t = (splitVal - ray.o[axis]) * invDir[axis];

            // Setup the new exit point

            int tmp = exPt++;
            // Possibly skip current entry point so not to overwrite the data
            if (exPt == enPt)
                exPt++;

            // Push values onto the stack
            stack[exPt].prev = tmp;
            stack[exPt].t = t;
            stack[exPt].node = farChild;
            stack[exPt].pb[axis] = splitVal;

            if(axis == 0) {
                stack[exPt].pb[1] = ray.o[1] + t * ray.d[1];
                stack[exPt].pb[2] = ray.o[2] + t * ray.d[2];
            } else if(axis == 1) {
                stack[exPt].pb[2] = ray.o[2] + t * ray.d[2];
                stack[exPt].pb[0] = ray.o[0] + t * ray.d[0];
            } else {
                stack[exPt].pb[0] = ray.o[0] + t * ray.d[0];
                stack[exPt].pb[1] = ray.o[1] + t * ray.d[1];
            }
        }

        // Dade - it looks like using mint/maxt here is faster than use the
        // inverse mailboxes
        ray.mint = max(stack[enPt].t, originalMint);
        ray.maxt = min(stack[exPt].t, originalMaxt);

        // Check for intersections inside leaf node
        u_int nPrimitives = currNode->nPrimitives();
        bool hit = false;

        // Dade - debugging code
        //std::stringstream ss;
        //ss<<"\n-----------------------------------------------------\n"<<
        //	"nPrims = "<<nPrimitives<<" hit = "<<hit<<
        //   //" ray.mint = "<<ray.mint<<" ray.maxt = "<<ray.maxt<<
        //    " tmin = "<<tmin<<" tmax = "<<tmax;
        //girlError(GIRL_NOERROR,GIRL_INFO,ss.str().c_str());

        if (nPrimitives == 1) {
            Primitive *pp = currNode->onePrimitive;

            if (pp->Intersect(ray, isect))
                hit = true;
        } else {
            Primitive **prims = currNode->primitives;
            for (u_int i = 0; i < nPrimitives; ++i) {
                Primitive *pp = prims[i];

                if (pp->Intersect(ray, isect))
                    hit = true;
            }
        }

        if (hit) {
			ray.mint = originalMint;
            return true;
        }

        // Pop from the stack
        enPt = exPt; // The signed distance intervals are adjacent

        // Retrieve the pointer to the next node, it is possible that ray traversal terminates
        currNode = stack[exPt].node;
        exPt = stack[enPt].prev;
    }

    ray.mint = originalMint;
    ray.maxt = originalMaxt;

    return false;
}

bool TaBRecKdTreeAccel::IntersectP(const Ray &ray) const {
    // Compute initial parametric range of ray inside kd-tree extent
    float t, tmin, tmax;
    if (!bounds.IntersectP(ray, &tmin, &tmax))
        return false;

    // Dade - Prepare the local Mailboxes. I'm going to use an inverse mailboxes
    // in order to be thread-safe
    TaBRecInverseMailboxes mailboxes;

    // Prepare to traverse kd-tree for ray
    Vector invDir(1.f/ray.d.x, 1.f/ray.d.y, 1.f/ray.d.z);
    #define MAX_TODO 64
    TaBRecKdNodeStack stack[MAX_TODO];
    int enPt = 0;
    stack[enPt].t = tmin;

    // Distinguish between internal and external origin
    if (tmin >= 0.0f)
        stack[enPt].pb = ray.o + ray.d * tmin;
    else
        stack[enPt].pb = ray.o;

    // Setup initial exit point in the stack
    int exPt = 1; // Pointer to the stack
    stack[exPt].t = tmax;
    stack[exPt].pb = ray.o + ray.d * tmax;
    stack[exPt].node = NULL; // Set termination flag

    const TaBRecKdAccelNode *currNode = &nodes[0];
    const TaBRecKdAccelNode *farChild;
    while (currNode != NULL) {
        while (!currNode->IsLeaf()) {
            // Retrieve position of splitting plane
            float splitVal = currNode->SplitPos();
            int axis = currNode->SplitAxis();

            if (stack[enPt].pb[axis] <= splitVal) {
                if (stack[exPt].pb[axis] <= splitVal) {
                    // Case N1, N2, N3, P5, Z2, and Z3
                    currNode = currNode + 1;
                    continue;
                }
                if (stack[enPt].pb[axis] == splitVal) {
                    currNode = &nodes[currNode->aboveChild];
                    continue; // case Z1
                }

                // Case N4

                farChild = &nodes[currNode->aboveChild];
                currNode = currNode + 1;
            } else {
                if (splitVal < stack[exPt].pb[axis]) {
                    // Case P1, P2, P3, and N5

                    currNode = &nodes[currNode->aboveChild];
                    continue;
                }

                // Case P4
                farChild = currNode + 1;
                currNode = &nodes[currNode->aboveChild];
            }

            // Case P4 or N4 . . . traverse both children

            // Signed distance to the splitting plane
            t = (splitVal - ray.o[axis]) * invDir[axis];

            // Setup the new exit point

            int tmp = exPt++;
            // Possibly skip current entry point so not to overwrite the data
            if (exPt == enPt)
                exPt++;

            // Push values onto the stack
            stack[exPt].prev = tmp;
            stack[exPt].t = t;
            stack[exPt].node = farChild;

            stack[exPt].pb[axis] = splitVal;
            if(axis == 0) {
                stack[exPt].pb[1] = ray.o[1] + t * ray.d[1];
                stack[exPt].pb[2] = ray.o[2] + t * ray.d[2];
            } else if(axis == 1) {
                stack[exPt].pb[2] = ray.o[2] + t * ray.d[2];
                stack[exPt].pb[0] = ray.o[0] + t * ray.d[0];
            } else {
                stack[exPt].pb[0] = ray.o[0] + t * ray.d[0];
                stack[exPt].pb[1] = ray.o[1] + t * ray.d[1];
            }
        }

        // Check for intersections inside leaf node
        u_int nPrimitives = currNode->nPrimitives();

        // Dade - debugging code
        //std::stringstream ss;
        //ss<<"\n-----------------------------------------------------\n"<<
        //       "nPrims = "<<nPrimitives<<
        //        " ray.mint = "<<ray.mint<<" ray.maxt = "<<ray.maxt;
        //girlError(GIRL_NOERROR,GIRL_INFO,ss.str().c_str());

        if (nPrimitives == 1) {
            Primitive *pp = currNode->onePrimitive;

            // Dade - check with the mailboxes if we need to do
            // the intersection test
            if (!mailboxes.alreadyChecked(pp)) {
                if (pp->IntersectP(ray))
                    return true;

                mailboxes.addChecked(pp);
            }
        } else {
            Primitive **prims = currNode->primitives;
            for (u_int i = 0; i < nPrimitives; ++i) {
                Primitive *pp = prims[i];

                // Dade - check with the mailboxes if we need to do
                // the intersection test
                if (!mailboxes.alreadyChecked(pp)) {
                    if (pp->IntersectP(ray))
                        return true;

                    mailboxes.addChecked(pp);
                }
            }
        }

        // Pop from the stack
        enPt = exPt; // The signed distance intervals are adjacent

        // Retrieve the pointer to the next node, it is possible that ray traversal terminates
        currNode = stack[exPt].node;
        exPt = stack[enPt].prev;
    }

    return false;
}

void TaBRecKdTreeAccel::GetPrimitives(vector<boost::shared_ptr<Primitive> > &primitives) {
	primitives.reserve(nPrims);
	for(u_int i=0; i<nPrims; i++) {
		primitives.push_back(prims[i]);
	}
}

Aggregate *TaBRecKdTreeAccel::CreateAccelerator(const vector<boost::shared_ptr<Primitive> > &prims,
        const ParamSet &ps) {
    int isectCost = ps.FindOneInt("intersectcost", 80);
    int travCost = ps.FindOneInt("traversalcost", 1);
    float emptyBonus = ps.FindOneFloat("emptybonus", 0.5f);
    int maxPrims = ps.FindOneInt("maxprims", 1);
    int maxDepth = ps.FindOneInt("maxdepth", -1);
    return new TaBRecKdTreeAccel(prims, isectCost, travCost,
            emptyBonus, maxPrims, maxDepth);
}

static DynamicLoader::RegisterAccelerator<TaBRecKdTreeAccel> r1("tabreckdtree");
static DynamicLoader::RegisterAccelerator<TaBRecKdTreeAccel> r2("kdtree");
