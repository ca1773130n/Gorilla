/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   kdtreecache.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - kdtreecache.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdio.h>
#include <string.h>
#include "tabreckdtreeaccel.h"

namespace girl
{

class GIRLCOREDLL KdTreeCache : public TaBRecKdTreeAccel {
public:
	// KdTreeAccel Public Methods
	KdTreeCache(const vector<boost::shared_ptr<Primitive> > &p,
		int icost, int scost,
		float ebonus, int maxp, int maxDepth, string file_name, bool _overwrite);
	~KdTreeCache() {};
	void WriteTree(int nodeNum, const BBox &bounds,
	    const vector<BBox> &primBounds,
		int *primNums, int nprims, int depth,
		TaBRecBoundEdge *edges[3],
		int *prims0, int *prims1, int badRefines = 0);

	//void ReadTree(MailboxPrim *mailboxPrims, MemoryArena &arena);
	void ReadTree(MemoryArena &arena);

	static Aggregate *CreateAccelerator(const vector<boost::shared_ptr<Primitive> > &prims, const ParamSet &ps);

private:
	void Post_WriteTree(int maxDepth);
	string filename;
	bool overwrite;
	FILE *kdnfp, *kdxfp;
	int prevLeafNum;
	int kdxnumber; // total number of integers (indices) in kdx file.
	int nodesnumber; // total number of nodes[] made.
};

}; // end of namespace