/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   bbox.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - bbox.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_BBOX_H
#define GIRL_BBOX_H

#include <iostream>
#include "vector.h"
#include "point.h"

namespace girl
{

class GIRLCOREDLL BBox {
public:
	// BBox Public Methods
	BBox() {
		pMin = Point( INFINITY,  INFINITY,  INFINITY);
		pMax = Point(-INFINITY, -INFINITY, -INFINITY);
	}
	BBox(const Point &p) : pMin(p), pMax(p) { }
	BBox(const Point &p1, const Point &p2) {
		pMin = Point(min(p1.x, p2.x),
					 min(p1.y, p2.y),
					 min(p1.z, p2.z));
		pMax = Point(max(p1.x, p2.x),
					 max(p1.y, p2.y),
					 max(p1.z, p2.z));
	}
	friend inline ostream &
		operator<<(ostream &os, const BBox &b);
	friend  BBox Union(const BBox &b, const Point &p);
	friend  BBox Union(const BBox &b, const BBox &b2);
	bool Overlaps(const BBox &b) const {
		bool x = (pMax.x >= b.pMin.x) && (pMin.x <= b.pMax.x);
		bool y = (pMax.y >= b.pMin.y) && (pMin.y <= b.pMax.y);
		bool z = (pMax.z >= b.pMin.z) && (pMin.z <= b.pMax.z);
		return (x && y && z);
	}
	bool Inside(const Point &pt) const {
		return (pt.x >= pMin.x && pt.x <= pMax.x &&
	            pt.y >= pMin.y && pt.y <= pMax.y &&
	            pt.z >= pMin.z && pt.z <= pMax.z);
	}
	void Expand(float delta) {
		pMin -= Vector(delta, delta, delta);
		pMax += Vector(delta, delta, delta);
	}
	float Volume() const {
		Vector d = pMax - pMin;
		return d.x * d.y * d.z;
	}
	int MaximumExtent() const {
		Vector diag = pMax - pMin;
		if (diag.x > diag.y && diag.x > diag.z)
			return 0;
		else if (diag.y > diag.z)
			return 1;
		else
			return 2;
	}
	void BoundingSphere(Point *c, float *rad) const;
	bool IntersectP(const Ray &ray,
	                float *hitt0 = NULL,
					float *hitt1 = NULL) const;
	// BBox Public Data
	Point pMin, pMax;
	

	
};

inline ostream &operator<<(ostream &os, const BBox &b) {
	os << b.pMin << " -> " << b.pMax;
	return os;
}


}//namespace girl

#endif //GIRL_BBOX_H
