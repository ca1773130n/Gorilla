/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   ray.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - ray.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_RAY_H
#define GIRL_RAY_H

#include <iostream>
#include "vector.h"
#include "point.h"

namespace girl
{

class GIRLCOREDLL   Ray {
public:
	// Ray Public Methods
	Ray(): mint(RAY_EPSILON), maxt(INFINITY), time(0.f) {}
	Ray(const Point &origin, const Vector &direction,
		float start = RAY_EPSILON, float end = INFINITY, float t = 0.f)
		: o(origin), d(direction), mint(start), maxt(end), time(t) { }
	Point operator()(float t) const { return o + d * t; }
	void GetDirectionSigns(int signs[3]) const {
		signs[0] = d.x < 0.f;
		signs[1] = d.y < 0.f;
		signs[2] = d.z < 0.f;
	}
	// Ray Public Data
	Point o;
	Vector d;
	mutable float mint, maxt;
	float time;
};

inline ostream &operator<<(ostream &os, Ray &r) {
	os << "org: " << r.o << "dir: " << r.d << " range [" <<
		r.mint << "," << r.maxt << "] time = " <<
		r.time;
	return os;
}

}//namespace girl

#endif //GIRL_RAY_H
