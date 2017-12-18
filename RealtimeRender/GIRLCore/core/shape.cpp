/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   shape.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - shape.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// shape.cpp*
#include "shape.h"
#include "primitive.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// Shape Method Definitions
Shape::Shape(const Transform &o2w, bool ro)
	: ObjectToWorld(o2w), WorldToObject(o2w.GetInverse()),
	reverseOrientation(ro),
	transformSwapsHandedness(o2w.SwapsHandedness()) {
}
Shape::Shape(const Transform &o2w, bool ro, boost::shared_ptr<Material> mat)
	: ObjectToWorld(o2w), WorldToObject(o2w.GetInverse()),
	reverseOrientation(ro),
	transformSwapsHandedness(o2w.SwapsHandedness()),
	material(mat)
{
	// Update shape creation statistics
	// radiance - disabled for threading // static StatsCounter nShapesMade("Geometry","Total shapes created");
	// radiance - disabled for threading // ++nShapesMade;
}

// PrimitiveSet Method Definitions
PrimitiveSet::PrimitiveSet(boost::shared_ptr<Aggregate> a) {
	a->GetPrimitives(primitives);

	initAreas();

	accelerator = a;
}
PrimitiveSet::PrimitiveSet(const vector<boost::shared_ptr<Primitive> > &p) {
	primitives = p;

	initAreas();

	// NOTE - ratow - Use accelerator for complex lights.
	if(primitives.size() <= 16) {
		accelerator = boost::shared_ptr<Primitive>((Primitive*)NULL);
		worldbound = WorldBound();
		// NOTE - ratow - Correctly expands bounds when pMin is not negative or pMax is not positive.
		worldbound.pMin -= (worldbound.pMax-worldbound.pMin)*0.01f;
		worldbound.pMax += (worldbound.pMax-worldbound.pMin)*0.01f;
	} else {
		accelerator = boost::shared_ptr<Primitive>(
				MakeAccelerator("kdtree", primitives, ParamSet()));
		if (!accelerator)
			girlError(GIRL_BUG,GIRL_SEVERE,"Unable to find \"kdtree\" accelerator");
	}
}
bool PrimitiveSet::Intersect(const Ray &ray, Intersection *in) const {
	if(accelerator) {
		if (!accelerator->Intersect(ray, in))
			return false;

		return true;
	} else if(worldbound.IntersectP(ray)) {
		// NOTE - ratow - Testing each shape for intersections again because the _PrimitiveSet_ can be non-planar.
		bool anyHit = false;
		for (u_int i = 0; i < primitives.size(); ++i) {
			if (primitives[i]->Intersect(ray, in)) {
				anyHit = true;
			}
		}
		return anyHit;
	} else {
		return false;
	}
}

bool PrimitiveSet::IntersectP(const Ray &ray) const {
	if(accelerator) {
		if (!accelerator->IntersectP(ray))
			return false;

		return true;
	} else if(worldbound.IntersectP(ray)) {
		for (u_int i = 0; i < primitives.size(); ++i) {
			if (primitives[i]->IntersectP(ray)) {
				return true;
			}
		}
		return false;
	} else {
		return false;
	}
}

void PrimitiveSet::initAreas() {
	area = 0;
	vector<float> areas;
	areas.reserve(primitives.size());
	for (u_int i = 0; i < primitives.size(); ++i) {
		float a = primitives[i]->Area();
		area += a;
		areas.push_back(a);
	}
	float prevCDF = 0;
	areaCDF.reserve(primitives.size());
	for (u_int i = 0; i < primitives.size(); ++i) {
		areaCDF.push_back(prevCDF + areas[i] / area);
		prevCDF = areaCDF[i];
	}
}
