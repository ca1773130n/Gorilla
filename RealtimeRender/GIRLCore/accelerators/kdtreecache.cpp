/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   kdtreecache.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - kdtreecache.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "kdtreecache.h"
#include "paramset.h"
#include "error.h"
#include "dynload.h"

#define KDNHEADING  "PR3: KDN file"
#define KDXHEADING  "PR3: KDX file"
#define HEADSIZE	64

using namespace girl;

// KdTreeCache Method Definitions
KdTreeCache::
    KdTreeCache(const vector<boost::shared_ptr<Primitive> > &p,
		int icost, int tcost,
		float ebonus, int maxp, int maxDepth, string file_name, bool _overwrite)
{
	// Check Time -- Start (begin)
	LARGE_INTEGER frequency;
	LARGE_INTEGER stopwatch_1;
	LARGE_INTEGER stopwatch_2;
	LARGE_INTEGER elapsed;
	QueryPerformanceFrequency(&frequency);  // Checking the HZ of the counter 
    QueryPerformanceCounter(&stopwatch_1);
	// Check Time -- Start (end)

	isectCost = icost;
	traversalCost = tcost;
	maxPrims = maxp;
	emptyBonus = ebonus;
	filename = file_name;
	overwrite = _overwrite;

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
	ss << "Building KDTreeCache, Primitives: " << nPrims;
	girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

	int FNAMESIZE = filename.size()+5;
	char *kdnfname = new char [FNAMESIZE];
	char *kdxfname = new char [FNAMESIZE];
	if (filename == "") { // no file name specified.
		// Build and Save
//		ss.str("");
//		ss << "We build a kd-tree from scratch and save it in tmp.kdn and tmp.kdx.";
//		girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());
		//printf("We build a kd-tree from scratch and save it in tmp.kdn and tmp.kdx.\n");
		strncpy_s(kdnfname, FNAMESIZE, "tmp.kdn\0",8);
		strncpy_s(kdxfname, FNAMESIZE, "tmp.kdx\0",8);
		//printf("kdnfname is %s, and kdxfname is %s.\n", kdnfname, kdxfname);
//		ss << "kdnfname is" << kdnfname << ", and kdxfname is " << kdxfname;
//		girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());
	}
	else {
		strncpy_s(kdnfname,FNAMESIZE, filename.c_str(), filename.size());
		strncpy_s(kdxfname,FNAMESIZE, filename.c_str(), filename.size());
		strncat(kdnfname, ".kdn\0",5);
		strncat(kdxfname, ".kdx\0",5);
//		printf("The kd-tree will be written in %s file and %s file.\n", kdnfname, kdxfname);
	}

	if ((filename != "") && (overwrite == FALSE) &&
		(fopen_s(&kdnfp, kdnfname, "rb") == 0) && (fopen_s(&kdxfp, kdxfname, "rb") == 0)) {

//			printf("We read from %s and %s\n", kdnfname, kdxfname);
			//  [11/13/2008 Sung-Soo Kim] commented for mailboxPrims compile error.
			// ReadTree(mailboxPrims, arena);
			ReadTree(arena);
	}
	else {
		if (fopen_s(&kdnfp, kdnfname, "wb") != 0) {
			printf("ERROR: cannot open %s file for wb.\n", kdnfname);
			exit(0);
		}
		if (fopen_s(&kdxfp, kdxfname, "wb") != 0) {
			printf("ERROR: cannot open %s.kdx file for wb.\n", kdxfname);
			fclose(kdnfp);
			exit(0);
		}
		// Write the headings for the files.
		char heading[HEADSIZE];
		strncpy_s(heading, HEADSIZE, KDNHEADING, sizeof(KDNHEADING));
		fwrite(heading, sizeof(char), strlen(heading), kdnfp);
		strncpy_s(heading, HEADSIZE, KDXHEADING, sizeof(KDXHEADING));
		fwrite(heading, sizeof(char), strlen(heading), kdxfp);
		// Write dummy number for the total number of integers (data) in kdx file.
		int idxnum = 0;
		fwrite(&idxnum, sizeof(int), 1, kdxfp);

		nodesnumber = 0;
		kdxnumber = 0;
		prevLeafNum = -1;
		WriteTree(0, bounds, primBounds, primNums, vPrims.size(), maxDepth, edges, prims0, prims1);
		Post_WriteTree(maxDepth);
	}

	fclose (kdnfp);
	fclose (kdxfp);

	// Free working memory for kd-tree construction
	delete[] primNums;
	for (int i = 0; i < 3; ++i)
		delete[] edges[i];
	delete[] prims0;
	delete[] prims1;

	// Check Time - End (begin)
	QueryPerformanceCounter(&stopwatch_2);
    elapsed.QuadPart = stopwatch_2.QuadPart - stopwatch_1.QuadPart;
    double elapsed_in_sec = (double)elapsed.QuadPart / frequency.QuadPart;
	//printf("Kd-Tree Building Time with file-writing or file-reading is %f seconds.\n", elapsed_in_sec);
	ss.str("");
	ss << "Acceleration structure construction time " << elapsed_in_sec << " seconds."; // << std::endl;
	girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

	// Check Time - End (end)
}
void KdTreeCache::WriteTree(int nodeNum,
        const BBox &nodeBounds,
		const vector<BBox> &allPrimBounds, int *primNums,
		int nPrims, int depth, TaBRecBoundEdge *edges[3],
		int *prims0, int *prims1, int badRefines) {
	// Write the primitive index file (.kdx file) while we build.
	assert(nodeNum == nextFreeNode); // NOBOOK
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
		//nodes[nodeNum].initLeaf(primNums, nPrims, mailboxPrims, arena);
		//  [11/13/2008 Sung-Soo Kim] use prims instead of mailboxPrims.
		nodes[nodeNum].initLeaf(primNums, nPrims, prims, arena);
		nodesnumber++;
		// Write dummy info for the aboveChild for the interior nodes before the current leaf node,
		// and then write the primitive indices for the current leaf node.
		if  (kdxfp != NULL) {
			// Write dummy values for the aboveChild space for the interior nodes.
			// prevLeafNum must be initialized as -1.
			int intnum = nodeNum-prevLeafNum - 1;
			prevLeafNum = nodeNum;
			if (intnum > 0) { // if there is at least one interior node between 2 leaf nodes;
				int *above = new int [intnum];
				memset(above, 0, intnum*sizeof(int));
				fwrite (above, sizeof(int), intnum, kdxfp);
				delete [] above;
				kdxnumber += intnum;
			}
			// Now write the primitive index list.
			int np = nodes[nodeNum].nPrimitives();
			fwrite (primNums , sizeof(primNums[0]), np, kdxfp);
			kdxnumber += np;
		}
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
			int otherAxis[3][2] = { {1,2}, {0,2}, {0,1} };
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
	assert(nBelow == nPrims && nAbove == 0); // NOBOOK
	// Create leaf if no good splits were found
	if (bestAxis == -1 && retries < 2) {
		++retries;
		axis = (axis+1) % 3;
		goto retrySplit;
	}
	if (bestCost > oldCost) ++badRefines;
	if ((bestCost > 4.f * oldCost && nPrims < 16) ||
		bestAxis == -1 || badRefines == 3) {
		//nodes[nodeNum].initLeaf(primNums, nPrims, mailboxPrims, arena);
		//  [11/13/2008 Sung-Soo Kim] prims instead of mailboxPrims.
		nodes[nodeNum].initLeaf(primNums, nPrims, prims, arena);
		nodesnumber++;
		// Write dummy info for the aboveChild for the interior nodes before the current leaf node,
		// and then write the primitive indices for the current leaf node.
		if  (kdxfp != NULL) {
			// Write dummy values for the aboveChild space for the interior nodes.
			// prevLeafNum must be initialized as -1.
			int intnum = nodeNum-prevLeafNum - 1;
			prevLeafNum = nodeNum;
			if (intnum > 0) { // if there is at least one interior node between 2 leaf nodes;
				int *above = new int [intnum];
				memset(above, 0, intnum*sizeof(int));
				fwrite (above, sizeof(int), intnum, kdxfp);
				delete [] above;
				kdxnumber += intnum;
			}
			// Now write the primitive index list.
			int np = nodes[nodeNum].nPrimitives();
			fwrite (primNums , sizeof(primNums[0]), np, kdxfp);
			kdxnumber += np;
		}
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
	nodesnumber++;
	BBox bounds0 = nodeBounds, bounds1 = nodeBounds;
	bounds0.pMax[bestAxis] = bounds1.pMin[bestAxis] = tsplit;
	WriteTree(nodeNum+1, bounds0,
		allPrimBounds, prims0, n0, depth-1, edges,
		prims0, prims1 + nPrims, badRefines);
	nodes[nodeNum].aboveChild = nextFreeNode;
	WriteTree(nodes[nodeNum].aboveChild, bounds1, allPrimBounds,
		prims1, n1, depth-1, edges,
		prims0, prims1 + nPrims, badRefines);
}
union info {
	float split;   // Interior
	u_int nPrims;  // Leaf
};
void KdTreeCache::Post_WriteTree(int maxDepth) {
	// Write the array nodes[] in sequence.
	// primitives are written immediately after the leaf node information.
	// Write in KDN file.
	if ((kdnfp == NULL) || (kdxfp == NULL)) {
		printf("ERROR in Post_Write_Tree: kdn or kdx file is not open.\n");
		if (kdnfp != NULL) fclose(kdnfp);
		else if (kdxfp != NULL) fclose(kdxfp);
		return;
	}
	printf("Post_WriteTree: nodesnumber is %d\n", nodesnumber);
	fwrite(&nodesnumber, sizeof(int), 1, kdnfp);
	info *content = new info [nodesnumber];

	for (int i=0; i < nodesnumber; i++) {
		if (nodes[i].IsLeaf()) {
			content[i].nPrims = nodes[i].nPrims;
		}
		else {
			content[i].split = nodes[i].split;
		}
	}
	fwrite(content, sizeof(union info), nodesnumber, kdnfp);
	delete[] content;

	// Update kdx file with the aboveChild information for interior nodes.
	rewind(kdxfp);
	fseek(kdxfp, strlen(KDXHEADING), SEEK_CUR);
	// Update the number of total data integers in kdx file.
	fwrite (&kdxnumber, sizeof(int), 1, kdxfp);
	// This part of code assumes that the kd-tree in nodes always end with leaf node.
	int *above = new int [maxDepth+1];
	int i=0, j;
	while (i < nodesnumber) {
		// To write the consecutive interior node info. at once.
		j=0;
		while (!nodes[i].IsLeaf()) { // while interior node
			above[j++] = (int) nodes[i++].aboveChild;
		}
		if (above[0] != 0) {
			fwrite (above, sizeof(int), j, kdxfp);
			above[0] = 0;
		}
		if (i >= nodesnumber) break;
		int skipnum=0;
		while (nodes[i].IsLeaf()) {
			skipnum += nodes[i++].nPrimitives();
		}
		fseek(kdxfp, skipnum*sizeof(int), SEEK_CUR);
	}
	delete[] above;
}
//void KdTreeCache::ReadTree(MailboxPrim *mailboxPrims, MemoryArena &arena) {
void KdTreeCache::ReadTree(MemoryArena &arena) {
	// Read the sequence of nodes[] from kdn and kdx files and build a kd-tree.
	// Check the headings: if this is the right file format.
	char heading[64];
	memset(heading, '\0', sizeof(heading));
	fread(heading, sizeof(char), strlen(KDNHEADING), kdnfp);
//	printf("Read from kdn file %d: %s\n", strlen(KDNHEADING), heading);
	if (strncmp(heading,KDNHEADING,strlen(KDNHEADING)) != 0) {
		printf("Error: illegal KDN file format.\n");
		return;
	}
	memset(heading, '\0', 32);
	fread(heading, sizeof(char), strlen(KDXHEADING), kdxfp);
//	printf("Read from kdx file %d: %s\n", strlen(KDXHEADING), heading);
	if (strncmp(heading,KDXHEADING,strlen(KDXHEADING)) !=0) {
		printf("Error: illegal KDX file format.\n");
		return;
	}

	// Check the total sizes to read 
	fread(&nodesnumber, sizeof(nodesnumber), 1, kdnfp);
//	printf("nodesnumber = %d\n", nodesnumber);
	fread(&kdxnumber, sizeof(kdxnumber), 1, kdxfp);
//	printf("kdxnumber = %d\n", kdxnumber);

	// Now read the content from KDN and KDX file.
	info *value = new info [nodesnumber];
	fread(value, sizeof(float), nodesnumber, kdnfp);
	int *idx = new int [kdxnumber];
	fread(idx, sizeof(int), kdxnumber, kdxfp);

	// Assign in nodes.
	TaBRecKdAccelNode *n = (TaBRecKdAccelNode *)AllocAligned((nodesnumber) *
			sizeof(TaBRecKdAccelNode));
	nodes = n;
	// i for kdn, j for kdx file.
	for (int i=0,j=0; i < nodesnumber; i++) {
		if ((value[i].nPrims & 3) == 3) { // If this is for a leaf
			nodes[i].nPrims = value[i].nPrims;
			int Number_prim = nodes[i].nPrimitives();
			int *primNums = new int [Number_prim];
			for (int k=0; k<Number_prim; k++) {
				primNums[k] = idx[j++];
			}
			//nodes[i].initLeaf (primNums,Number_prim, mailboxPrims, arena);
			//  [11/13/2008 Sung-Soo Kim] prims instead of mailboxPrims.
			nodes[i].initLeaf (primNums,Number_prim, prims, arena);
			delete [] primNums;
		}
		else { // if this is for an interior node
			nodes[i].split = value[i].split;
			nodes[i].aboveChild = (u_int) idx[j++];
		}
	}
	delete [] value;
	delete [] idx;
}

Aggregate * KdTreeCache::CreateAccelerator(const vector<boost::shared_ptr<Primitive> > &prims,
		const ParamSet &ps) {
	string filename = ps.FindOneString("filename","defaultCache");
	bool overwrite = ps.FindOneBool("overwrite",FALSE);
	int isectCost = ps.FindOneInt("intersectcost", 80);
	int travCost = ps.FindOneInt("traversalcost", 1);
	float emptyBonus = ps.FindOneFloat("emptybonus", 0.5f);
	int maxPrims = ps.FindOneInt("maxprims", 1);
	int maxDepth = ps.FindOneInt("maxdepth", -1);
	return new KdTreeCache(prims, isectCost, travCost, emptyBonus, maxPrims, maxDepth, filename, overwrite);
}

static DynamicLoader::RegisterAccelerator<KdTreeCache> r("kdtreecache");