/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   bruteforce.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - bruteforce.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// Brute force ray intersection (de)accelerator.
// Only useful for debugging as this is way slow on big scenes.

// bruteforce.cpp*
#include "bruteforce.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// BruteForceAccel Method Definitions
BruteForceAccel::BruteForceAccel(const vector<boost::shared_ptr<Primitive> > &p) {
	PrimitiveRefinementHints refineHints(false);
	for (u_int i = 0; i < p.size(); ++i) {
		if(p[i]->CanIntersect())
			prims.push_back(p[i]);
		else
			p[i]->Refine(prims, refineHints, p[i]);
	}
	// Compute bounds
	for (u_int i = 0; i < prims.size(); ++i)
		bounds = Union(bounds, prims[i]->WorldBound());
}

BruteForceAccel::~BruteForceAccel() {
}

BBox BruteForceAccel::WorldBound() const {
	return bounds;
}

bool BruteForceAccel::Intersect(const Ray &ray,
                          Intersection *isect) const {
	bool hitSomething = false;

	if (!bounds.IntersectP(ray))
		return false;

	for (u_int i = 0; i < prims.size(); ++i) {
		hitSomething |= prims[i]->Intersect(ray, isect);
	}

	return hitSomething;
}

bool BruteForceAccel::IntersectP(const Ray &ray) const {
	if (!bounds.IntersectP(ray))
		return false;

	for (u_int i = 0; i < prims.size(); ++i) {
		if(prims[i]->IntersectP(ray))
			return true;
	}

	return false;
}

void BruteForceAccel::GetPrimitives(vector<boost::shared_ptr<Primitive> > &primitives) {
	primitives.reserve(prims.size());
	for(u_int i=0; i < prims.size(); i++) {
		primitives.push_back(prims[i]);
	}
}

Aggregate* BruteForceAccel::CreateAccelerator(const vector<boost::shared_ptr<Primitive> > &prims,
		const ParamSet &ps) {
	return new BruteForceAccel(prims);
}

static DynamicLoader::RegisterAccelerator<BruteForceAccel> r("none");
