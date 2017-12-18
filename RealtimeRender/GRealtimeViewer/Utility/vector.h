#pragma once

#include <math.h>
#include <stdlib.h>
#include <cmath>
#include <algorithm>
using std::min;
using std::max;
using std::swap;
using std::sort;

using namespace std;

#define GLT_TRUE  			1
#define GLT_FALSE 			0

#ifndef M_PI
#define M_PI			(3.14159265358979323846)
#endif
#define M_PI_DIV_180	(0.017453292519943296)
#define M_INV_PI_DIV_180 (57.2957795130823229)

/*--------------------------------------------------------------------------*/
/* MACROS                                                                   */
/*------------------------- -------------------------------------------------*/

/* compute radian from degree */
#define GLT_TORAD(d) 			((d) * (M_PI_DIV_180))

/* compute degree from radian */
#define GLT_TODEG(r) 			((r) * (M_INV_PI_DIV_180))

/* return absolute value */
#define GLT_ABS(a) 			(((a) > 0) ? (a) : (-(a)))

/* return maximum of two numbers */
#define GLT_MAX(a,b) 			(((a) > (b)) ? (a) : (b))

/* return maximum of two numbers */
#define GLT_MIN(a,b) 			(((a) >= (b)) ? (b) : (a))

/* if given two strings are the same, return true */
#define GLT_SAME_STRING(a,b) 		(!strcmp((a),(b)))

/* compute distance between two 2D points (x1,y1) (x2,y2) */
#define GLT_DIST2(x1,y1,x2,y2) 		(sqrt(((x1)-(x2))*((x1)-(x2))+\
	((y1)-(y2))*((y1)-(y2))))

/* convert a floating point color into unsigned byte type  
the range of color value is scaled from [0.0 ... 1.0] to [0..255] */
#define GLT_COLOR_UBYTE(c)              ((((c) * 255.0)>(255.0)) ? ((GLTUByte)(255)) : ((GLTUByte)((c) * 255.0)))


/*--------------------------------------------------------------------------*/
/* TYPES                                                                    */
/*--------------------------------------------------------------------------*/
typedef float				GLfloat;
typedef GLfloat        		GLTVector2[2];
typedef GLfloat        		GLTVector3[3];
typedef GLfloat        		GLTVector4[4];
typedef GLfloat        		GLTMatrix[16];
typedef unsigned char		GLTUByte;
typedef unsigned char		GLTUByteColor[3];

/*--------------------------------------------------------------------------*/
/* ENUM TYPES                                                               */
/*--------------------------------------------------------------------------*/

/* distinguish 3D axes */
typedef enum {
	GLT_X = 0,
	GLT_Y,
	GLT_Z
} GLTAxisType;

/* vector.c */
void GLTMatInv(GLTMatrix M,GLTMatrix out);
float GLTVecDot(GLfloat* V1, GLfloat* V2, int D);
int GLTVecIsSame(GLfloat *V1, GLfloat *V2, int D);
void GLTVecCopy(GLfloat *V1, GLfloat *V2, int D);
void GLTVecAdd(GLfloat *V1, GLfloat *V2, GLfloat *V3, int D);
void GLTVecSub(GLfloat *V1, GLfloat *V2, GLfloat *V3, int D);
void GLTVecSMul(GLfloat *V1, GLfloat A, GLfloat *V2, int D);
void GLTVecNeg(GLfloat *V1, GLfloat *V2, int D);
GLfloat GLTVecNorm(GLfloat *V, int D);
void GLTVecNormalize(GLfloat *V, int D);
GLfloat GLTVecSProd(GLfloat *V1, GLfloat *V2, int D);
GLfloat GLTVecVProd2(GLTVector2 V1, GLTVector2 V2);
void GLTVecVProd3(GLTVector3 V1, GLTVector3 V2, GLTVector3 V3);
void GLTVec4To3(GLTVector4 V1, GLTVector3 V2);
void GLTMatUnit(GLTMatrix M);
void GLTMatCopy(GLTMatrix M1, GLTMatrix M2);
void GLTMatAdd(GLTMatrix M1, GLTMatrix M2, GLTMatrix M3);
void GLTMatSub(GLTMatrix M1, GLTMatrix M2, GLTMatrix M3);
void GLTMatMul(GLTMatrix M1, GLTMatrix M2, GLTMatrix M3);
void GLTMatTranspose(GLTMatrix M1, GLTMatrix M2);
GLfloat GLTMatDet(GLTMatrix M);
void GLTMatVecMul(GLTMatrix M, GLTVector4 V1, GLTVector4 V2);
void GLTVecMatMul(GLTVector4 V1, GLTMatrix M, GLTVector4 V2);
void GLTTrTrans3(GLTVector3 V1, GLTVector3 V2, GLTVector3 T);
void GLTTrRot3(GLTVector3 V1, GLTVector3 V2, GLfloat R, int Axis);
void GLTTrScale3(GLTVector3 V1, GLTVector3 V2, GLTVector3 S);
void GLTTrMakeMatTrans(GLTMatrix M, GLTVector3 T);
void GLTTrMakeMatRot(GLTMatrix M, GLfloat R, int Axis);
void GLTTrMakeMatScale(GLTMatrix M, GLTVector3 S);
/*
class Point;
class Normal;

class  Vector {
public:
	// Vector Public Methods
	Vector(float _x=0, float _y=0, float _z=0)
		: x(_x), y(_y), z(_z) {
	}
	explicit Vector(const Point &p);
	Vector operator+(const Vector &v) const {
		return Vector(x + v.x, y + v.y, z + v.z);
	}

	Vector& operator+=(const Vector &v) {
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	Vector operator-(const Vector &v) const {
		return Vector(x - v.x, y - v.y, z - v.z);
	}

	Vector& operator-=(const Vector &v) {
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	bool operator==(const Vector &v) const {
		return x == v.x && y == v.y && z == v.z;
	}
	Vector operator*(float f) const {
		return Vector(f*x, f*y, f*z);
	}

	Vector &operator*=(float f) {
		x *= f; y *= f; z *= f;
		return *this;
	}
	Vector operator/(float f) const {
		//		BOOST_ASSERT(f!=0);
		float inv = 1.f / f;
		return Vector(x * inv, y * inv, z * inv);
	}

	Vector &operator/=(float f) {
		//		BOOST_ASSERT(f!=0);
		float inv = 1.f / f;
		x *= inv; y *= inv; z *= inv;
		return *this;
	}
	Vector operator-() const {
		return Vector(-x, -y, -z);
	}
	float operator[](int i) const {
		//		BOOST_ASSERT(i >= 0 && i <= 2);
		return (&x)[i];
	}

	float &operator[](int i) {
		//		BOOST_ASSERT(i >= 0 && i <= 2);
		return (&x)[i];
	}
	float LengthSquared() const { return x*x + y*y + z*z; }
	float Length() const { return sqrtf(LengthSquared()); }
	explicit Vector(const Normal &n);
	// Vector Public Data
	float x, y, z;

private:
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & x;
		ar & y;
		ar & z;
	}
};
*/


/*
// Global Inline Functions
template<class T> inline T Lerp(float t, T v1, T v2) {
	return (1.f - t) * v1 + t * v2;
}
template<class T> inline T Clamp(T val, T low, T high) {
	return val > low ? (val < high ? val : high) : low;
}
inline int Mod(int a, int b) {
	// note - radiance - added 0 check to prevent divide by zero error(s)
	if(a==0) a=1;
	if(b==0) b=1;

	int n = int(a/b);
	a -= n*b;
	if (a < 0)
		a += b;
	return a;
}
inline float Radians(float deg) {
	return ((float)M_PI/180.f) * deg;
}
inline float Degrees(float rad) {
	return (180.f/(float)M_PI) * rad;
}
inline float Log2(float x) {
	return logf(x) / logf(2.f);
}
inline int Log2Int(float v) {
#if 0
	return ((*reinterpret_cast<int *>(&v)) >> 23) - 127;
#else
#define _doublemagicroundeps	      (.5-1.4e-11)
	return int(Log2(v) + _doublemagicroundeps);
#endif
}
inline bool IsPowerOf2(int v) {
	return (v & (v - 1)) == 0;
}
inline unsigned int RoundUpPow2(unsigned int v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	return v+1;
}
#if (defined(__linux__) && defined(__i386__)) || defined(WIN32)
//#define FAST_INT 1
#endif
#define _doublemagicroundeps	      (.5-1.4e-11)
//almost .5f = .5f - 1e^(number of exp bit)
inline int Round2Int(double val) {
#ifdef FAST_INT
#define _doublemagic			double (6755399441055744.0)
	//2^52 * 1.5,  uses limited precision to floor
	val		= val + _doublemagic;
	return (reinterpret_cast<long*>(&val))[0];
#else
	return int (val+_doublemagicroundeps);
#endif
}
template<class T> inline int Float2Int(T val) {
#ifdef FAST_INT
	return (val<0) ?  Round2Int(val+_doublemagicroundeps) :
		Round2Int(val-_doublemagicroundeps);
#else
	return (int)val;
#endif
}
template<class T> inline int Floor2Int(T val) {
#ifdef FAST_INT
	return Round2Int(val - _doublemagicroundeps);
#else
	return (int)floor(val);
#endif
}
template<class T> inline int Ceil2Int(T val) {
#ifdef FAST_INT
	return Round2Int(val + _doublemagicroundeps);
#else
	return (int)ceil(val);
#endif
}
inline bool Quadratic(float A, float B, float C, float *t0,
					  float *t1) {
						  // Find quadratic discriminant
						  float discrim = B * B - 4.f * A * C;
						  if (discrim < 0.) return false;
						  float rootDiscrim = sqrtf(discrim);
						  // Compute quadratic _t_ values
						  float q;
						  if (B < 0) q = -.5f * (B - rootDiscrim);
						  else       q = -.5f * (B + rootDiscrim);
						  *t0 = q / A;
						  *t1 = C / q;
						  if (*t0 > *t1) swap(*t0, *t1);
						  return true;
}
inline float SmoothStep(float min, float max, float value) {
	float v = Clamp((value - min) / (max - min), 0.f, 1.f);
	return v * v * (-2.f * v  + 3.f);
}





inline Vector operator*(float f, const Vector &v) {
	return v*f;
}

inline float Dot(const Vector &v1, const Vector &v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline float AbsDot(const Vector &v1, const Vector &v2) {
	return fabsf(Dot(v1, v2));
}

inline Vector Cross(const Vector &v1, const Vector &v2) {
	return Vector((v1.y * v2.z) - (v1.z * v2.y),
		(v1.z * v2.x) - (v1.x * v2.z),
		(v1.x * v2.y) - (v1.y * v2.x));
}

inline Vector Normalize(const Vector &v) {
	return v / v.Length();
}

inline void CoordinateSystem(const Vector &v1, Vector *v2, Vector *v3) {
	if (fabsf(v1.x) > fabsf(v1.y)) {
		float invLen = 1.f / sqrtf(v1.x*v1.x + v1.z*v1.z);
		*v2 = Vector(-v1.z * invLen, 0.f, v1.x * invLen);
	} else {
		float invLen = 1.f / sqrtf(v1.y*v1.y + v1.z*v1.z);
		*v2 = Vector(0.f, v1.z * invLen, -v1.y * invLen);
	}
	*v3 = Cross(v1, *v2);
}

inline Vector SphericalDirection(float sintheta, float costheta, float phi) {
	return Vector(sintheta * cosf(phi), sintheta * sinf(phi), costheta);
}

inline Vector SphericalDirection(float sintheta, float costheta, float phi,
								 const Vector &x, const Vector &y, const Vector &z) {
									 return sintheta * cosf(phi) * x + sintheta * sinf(phi) * y +
										 costheta * z;
}

inline float SphericalTheta(const Vector &v) {
	return acosf(Clamp(v.z, -1.f, 1.f));
}

inline float SphericalPhi(const Vector &v) {
	float p = atan2f(v.y, v.x);
	return (p < 0.f) ? p + 2.f*M_PI : p;
}
*/