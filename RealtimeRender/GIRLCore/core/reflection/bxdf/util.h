/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   util.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - util.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#ifndef _BRDF_UTIL_H
#define _BRDF_UTIL_H

/*

util.h
Misc mathematical utility functions
*/

#ifndef PI
#define PI 3.14159265358979323846
#endif

#ifndef SQRT2
#define SQRT2 1.41421356237309504880
#endif

#define EPS 0.0001

#undef SQR
#undef CUBE
#undef MIN
#undef MAX
#undef ABS

template <class T> inline T deg2rad(const T &x) { return PI/180.0*x; }
template <class T> inline T rad2deg(const T &x) { return 180.0/PI*x; }
template <class T> inline int SGN(const T &x) { return x<0 ? -1 : 1; }
template <class T> inline T SQR(const T &x) { return x*x; }
template <class T> inline T CUBE(const T &x) { return x*x*x; }
template <class T> inline const T& MIN(const T &x, const T &y) { return x<y ? x : y; }
template <class T> inline const T& MAX(const T &x, const T &y) { return x>y ? x : y; }

// Note that this does something sensible even when x is NaN
template <class T>
inline const T& CLAMP(const T &x, const T &min, const T &max)
{
	return (x > min) ? (x > max ? max : x) : min;
}

template <class T>
inline T compute_theta(const T &x, const T &y)
{
	T r = hypot(x,y);

	if (r >= 1.0)
		return (PI/2.0);
	return asin(r);
}

template <class T>
inline T compute_phi(const T &x, const T &y)
{
	return atan2(y,x);
}


template <class T>
inline void clamp_to_circle(T &x, T &y)
{
	T r2=SQR(x)+SQR(y);
	if (r2 > 1.0-EPS) {
		x /= (sqrt(r2)+EPS);
		y /= (sqrt(r2)+EPS);
	}
}

/* (a,b,c) cross (d,e,f) -> (g,h,i) */
template <class T>
inline void FindCrossProd(const T &a, const T &b, const T &c,
			  const T &d, const T &e, const T &f,
			        T &g,       T &h,       T &i)
{
	g = b*f-c*e;
	h = c*d-a*f;
	i = a*e-b*d;
}


template <class T>
inline void Normalize(T &a, T &b, T &c)
{
	T r=sqrt(SQR(a)+SQR(b)+SQR(c));
	if (r < EPS*EPS) {
		a = 0.0;
		b = 0.0;
		c = 1.0;
		return;
	}
	a /= r;
	b /= r;
	c /= r;
}

//  [11/28/2008 Sung-Soo Kim] remove plaform dependency
#if defined(WIN32) || defined(WIN64)
#define strcasecmp(x,y) stricmp((x),(y))
#define strncasecmp(x,y,z) strnicmp((x),(y),(z))
#else
#define stricmp(x,y) strcasecmp((x),(y))
#define strnicmp(x,y,z) strncasecmp((x),(y),(z))
#endif

double erf(double x);
double erfc(double x);

#endif
