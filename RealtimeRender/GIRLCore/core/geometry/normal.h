/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   normal.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - normal.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_NORMAL_H
#define GIRL_NORMAL_H

#include <iostream>
#include "vector.h"

namespace girl
{


class GIRLCOREDLL   Normal {
	friend class boost::serialization::access;
public:
	// Normal Methods
	Normal(float _x=0, float _y=0, float _z=0)
		: x(_x), y(_y), z(_z) {}
	Normal operator-() const {
		return Normal(-x, -y, -z);
	}
	Normal operator+ (const Normal &v) const {
		return Normal(x + v.x, y + v.y, z + v.z);
	}
	
	Normal& operator+=(const Normal &v) {
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	Normal operator- (const Normal &v) const {
		return Normal(x - v.x, y - v.y, z - v.z);
	}
	
	Normal& operator-=(const Normal &v) {
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	Normal operator* (float f) const {
		return Normal(f*x, f*y, f*z);
	}
	
	Normal &operator*=(float f) {
		x *= f; y *= f; z *= f;
		return *this;
	}
	Normal operator/ (float f) const {
		float inv = 1.f/f;
		return Normal(x * inv, y * inv, z * inv);
	}
	
	Normal &operator/=(float f) {
		float inv = 1.f/f;
		x *= inv; y *= inv; z *= inv;
		return *this;
	}
	float LengthSquared() const { return x*x + y*y + z*z; }
	float Length() const        { return sqrtf(LengthSquared()); }
	
	explicit Normal(const Vector &v)
	  : x(v.x), y(v.y), z(v.z) {}
	float operator[](int i) const { return (&x)[i]; }
	float &operator[](int i) { return (&x)[i]; }
	// Normal Public Data
	float x,y,z;

private:
	template<class Archive>
			void serialize(Archive & ar, const unsigned int version)
			{
				ar & x;
				ar & y;
				ar & z;
			}
};

inline Normal operator*(float f, const Normal &n) {
	return Normal(f*n.x, f*n.y, f*n.z);
}

inline Vector::Vector(const Normal &n)
  : x(n.x), y(n.y), z(n.z) { }

inline ostream &operator<<(ostream &os, const Normal &v) {
	os << v.x << ", " << v.y << ", " << v.z;
	return os;
}

}//namespace girl

#endif //GIRL_NORMAL_H
