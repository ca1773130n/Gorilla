/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   shape.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - shape.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_SHAPE_H
#define GIRL_SHAPE_H
// shape.h*
#include "girl.h"
#include "primitive.h"
#include "geometry.h"
#include "error.h"

namespace girl
{

// Lotus - A primitive implementation compatible with the old PBRT Shape class
// Shape Declarations
class GIRLCOREDLL  Shape : public Primitive {
public:
	Shape(const Transform &o2w, bool ro);
	Shape(const Transform &o2w, bool ro,
			boost::shared_ptr<Material> material);

	void SetMaterial(boost::shared_ptr<Material> mat) { this->material = mat; }
	boost::shared_ptr<Material> GetMaterial() const { return material; }

	BBox WorldBound() const { return ObjectToWorld(ObjectBound()); }
	void Refine(vector<boost::shared_ptr<Primitive> > &refined,
			const PrimitiveRefinementHints& refineHints,
			boost::shared_ptr<Primitive> thisPtr)
	{
		vector<boost::shared_ptr<Shape> > todo;
		Refine(todo); // Use shape refine method
		for(u_int i=0; i<todo.size(); i++) {
			boost::shared_ptr<Shape> shape = todo[i];
			shape->SetMaterial(this->material);
			if (shape->CanIntersect()) {
				refined.push_back(shape);
			}
			else {
				// Use primitive refine method
				shape->Refine(refined, refineHints, shape);
			}
		}
	}

	bool CanIntersect() const { return true; }
	bool Intersect(const Ray &r, Intersection *isect) const {
		float thit;
		if (!Intersect(r, &thit, &isect->dg))
			return false;
		isect->dg.AdjustNormal(reverseOrientation, transformSwapsHandedness);
		isect->Set(WorldToObject, this, material.get());
		r.maxt = thit;
		return true;
	}

	void GetShadingGeometry(const Transform &obj2world,
			const DifferentialGeometry &dg,
			DifferentialGeometry *dgShading) const
	{
		*dgShading = dg;
	}

	bool CanSample() const { return true; }

	// Old PBRT Shape interface methods
	virtual BBox ObjectBound() const {
		girlError(GIRL_BUG,GIRL_SEVERE,"Unimplemented Shape::ObjectBound method called!");
		return BBox();
	}
	virtual void Refine(vector<boost::shared_ptr<Shape> > &refined) const {
		girlError(GIRL_BUG,GIRL_SEVERE,"Unimplemented Shape::Refine() method called");
	}
	virtual bool Intersect(const Ray &ray, float *t_hitp,
			DifferentialGeometry *dg) const
	{
		girlError(GIRL_BUG,GIRL_SEVERE,"Unimplemented Shape::Intersect() method called");
		return false;
	}
	// Shape data
	const Transform ObjectToWorld, WorldToObject;
	const bool reverseOrientation, transformSwapsHandedness;
protected:
	boost::shared_ptr<Material> material;
};

class GIRLCOREDLL  PrimitiveSet : public Primitive {
public:
	// PrimitiveSet Public Methods
	PrimitiveSet(boost::shared_ptr<Aggregate> a);
	PrimitiveSet(const vector<boost::shared_ptr<Primitive> > &p);

	BBox WorldBound() const { return worldbound; }
	bool CanIntersect() const {
		for (u_int i = 0; i < primitives.size(); ++i)
			if (!primitives[i]->CanIntersect()) return false;
		return true;
	}
	bool Intersect(const Ray &r, Intersection *in) const;
	bool IntersectP(const Ray &r) const;

	bool CanSample() const {
		for (u_int i = 0; i < primitives.size(); ++i)
			if (!primitives[i]->CanSample()) return false;
		return true;
	}
	Point Sample(float u1, float u2, float u3, Normal *Ns) const {
		u_int sn;
		if( primitives.size() <= 16) {
			for (sn = 0; sn < primitives.size()-1; ++sn)
				if (u3 < areaCDF[sn]) break;
		}
		else {
			sn = Clamp(
				(u_int)(std::upper_bound(areaCDF.begin(), areaCDF.end(), u3) - areaCDF.begin()),
				(u_int)(0), (u_int)(primitives.size() - 1));
		}
		return primitives[sn]->Sample(u1, u2, u3, Ns);
	}
	float Area() const { return area; }
private:
	void initAreas();

	// PrimitiveSet Private Data
	float area;
	vector<float> areaCDF;
	vector<boost::shared_ptr<Primitive> > primitives;
	BBox worldbound;
	boost::shared_ptr<Primitive> accelerator;
};

}//namespace girl

#endif // GIRL_SHAPE_H
