/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   mycomplex.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - mycomplex.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#ifndef _MYCOMPLEX_H
#define _MYCOMPLEX_H

/*
mycomplex.h
Unfortunately, <complex.h>, <complex>, and friends are not portable, since much
of the world still has not adopted the C++ standard library.  Therefore, we
roll our own...  Note that this is largely based on the code that comes with
G++, and hence is covered under the GPL.

Not implemented: all trig functions, all exponential/log/pow functions, IO
*/

#include <math.h>

#undef TRUE
#undef FALSE

#ifdef _BOOL
#define BOOL bool
#define TRUE true
#define FALSE false
#else
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif

class complex {
private:
	double re, im;
public:
	complex(double _re=0, double _im=0) : re(_re), im(_im) {}
	complex& operator += (const complex&);
	complex& operator -= (const complex&);
	complex& operator *= (const complex&);
	complex& operator /= (const complex&);
	double real() const { return re; }
	double imag() const { return im; }

	friend double real (const complex&);
	friend double imag (const complex&);
	friend complex operator + (const complex&, const complex&);
	friend complex operator - (const complex&, const complex&);
	friend complex operator * (const complex&, const complex&);
	friend complex operator / (const complex&, const complex&);
	friend complex operator + (const complex&);
	friend complex operator - (const complex&);
	friend BOOL operator == (const complex&, const complex&);
	friend BOOL operator != (const complex&, const complex&);
	friend complex polar (double, double);
};

inline double real(const complex& c)
{
	return c.real();
}


inline double imag(const complex& c)
{
	return c.imag();
}


inline double abs(complex& x)
{
	return hypot(real(x), imag(x));
}


inline double arg(complex& x)
{
	return atan2(imag(x), real(x));
}


inline complex polar(double r, double th)
{
	return complex (r*cos(th), r*sin(th));
}


inline complex conj(const complex& x)
{
	return complex(real(x), -imag(x));
}


inline double norm(const complex& x)
{
	return real(x)*real(x) + imag(x)*imag(x);
}


inline complex&
complex::operator += (const complex& c)
{
	re += c.re;
	im += c.im;
	return *this;
}


inline complex&
complex::operator -= (const complex& c)
{
	re -= c.re;
	im -= c.im;
	return *this;
}


inline complex&
complex::operator *= (const complex& c)
{
	double tmp = re*c.re - im*c.im;
	im = re*c.im + im*c.re;
	re = tmp;
	return *this;
}

inline double ABS(const double &x) { return x<0 ? -x : x; }
inline double ABS(const complex &x) { return x.real() < 0 ? -x.real() : x.real();}

inline complex&
complex::operator /= (const complex& c)
{
	double ar = ABS(c.re);
	double ai = ABS(c.im);
	double nr, ni, t, d;
	if (ar <= ai) {
		t = c.re / c.im;
		d = c.im * (1 + t*t);
		nr = (re * t + im) / d;
		ni = (im * t - re) / d;
	} else {
		t = c.im / c.re;
		d = c.re * (1 + t*t);
		nr = (re + im * t) / d;
		ni = (im - re * t) / d;
	}
	re = nr;
	im = ni;
	return *this;
}


inline complex
operator + (const complex& x, const complex& y)
{
	return complex(real(x)+real(y), imag(x)+imag(y));
}


inline complex
operator - (const complex& x, const complex& y)
{
	return complex(real(x)-real(y), imag(x)-imag(y));
}



inline complex
operator * (const complex& x, const complex& y)
{
	return complex(real(x)*real(y) - imag(x)*imag(y),
				   real(x)*imag(y) + imag(x)*real(y));
}


inline complex
operator / (const complex& x, const complex& y)
{
	double ar = ABS(real(y));
	double ai = ABS(imag(y));
	double nr, ni, t, d;
	if (ar <= ai) {
		t = real(y) / imag(y);
		d = imag(y) * (1+t*t);
		nr = (real(x)*t + imag(x))/d;
		ni = (imag(x)*t - real(x))/d;
	} else {
		t = imag(y) / real(y);
		d = real(y) * (1+t*t);
		nr = (real(x) + imag(x)*t)/d;
		ni = (imag(x) - real(x)*t)/d;
	}
	return complex(nr, ni);
}


inline complex
operator + (const complex& x)
{
	return x;
}


inline complex
operator - (const complex& x)
{
	return complex(-real(x), -imag(x));
}


inline BOOL
operator == (const complex& x, const complex& y)
{
	return ((real(x) == real(y)) && (imag(x) == imag(y)));
}


inline BOOL
operator != (const complex& x, const complex& y)
{
	return ((real(x) != real(y)) || (imag(x) != imag(y)));
}

inline complex sqrt(const complex& x)
{
	double r = ABS(x);
	double nr, ni;
	if (r == 0.0)
		nr = ni = r;
	else if (real (x) > 0) {
		nr = sqrt(0.5 * (r + real(x)));
		ni = imag(x) / nr / 2;
	} else {
		ni = sqrt(0.5 * (r - real(x)));
		if (imag(x) < 0)
        		ni = - ni;
		nr = imag(x) / ni / 2;
	}
	return complex (nr, ni);
}

#endif
