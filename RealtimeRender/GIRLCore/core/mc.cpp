/***************************************************************************
 *   Copyright (C) 1998-2008 by authors (see AUTHORS.txt )                 *
 *                                                                         *
 *   This file is part of GirlRender.                                       *
 *                                                                         *
 *   Girl Renderer is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Girl Renderer is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *   This project is based on PBRT ; see http://www.pbrt.org               *
 *   Girl Renderer website : http://www.girlrender.net                       *
 ***************************************************************************/

// mc.cpp*
#include "girl.h"
#include "geometry.h"
#include "shape.h"
#include "mc.h"
#include "volume.h"

namespace girl
{

// MC Function Definitions
void ComputeStep1dCDF(float *f, int nSteps, float *c,
		float *cdf) {
	// Compute integral of step function at $x_i$
	int i;
	cdf[0] = 0.;
	for (i = 1; i < nSteps+1; ++i)
		cdf[i] = cdf[i-1] + f[i-1] / nSteps;
	// Transform step function integral into cdf
	*c = cdf[nSteps];
	for (i = 1; i < nSteps+1; ++i)
		cdf[i] /= *c;
}
float SampleStep1d(float *f, float *cdf, float c,
		int nSteps, float u, float *pdf) {
	// Find surrounding cdf segments
	float *ptr = std::lower_bound(cdf, cdf+nSteps+1, u);
	int offset = (int) (ptr-cdf-1);
	// Return offset along current cdf segment
	u = (u - cdf[offset]) / (cdf[offset+1] - cdf[offset]);
	*pdf = f[offset] / c;
	return (offset + u) / nSteps;
}
void RejectionSampleDisk(const TsPack *tspack, float *x, float *y) {
	float sx, sy;
	do {
		sx = 1.f - 2.f * tspack->rng->floatValue();
		sy = 1.f - 2.f * tspack->rng->floatValue();
	} while (sx*sx + sy*sy > 1.f);
	*x = sx;
	*y = sy;
}
 Vector UniformSampleHemisphere(float u1, float u2) {
	float z = u1;
	float r = sqrtf(max(0.f, 1.f - z*z));
	float phi = 2 * M_PI * u2;
	float x = r * cosf(phi);
	float y = r * sinf(phi);
	return Vector(x, y, z);
}
 float UniformHemispherePdf(float theta, float phi) {
	return INV_TWOPI;
}
 Vector UniformSampleSphere(float u1, float u2) {
	float z = 1.f - 2.f * u1;
	float r = sqrtf(max(0.f, 1.f - z*z));
	float phi = 2.f * M_PI * u2;
	float x = r * cosf(phi);
	float y = r * sinf(phi);
	return Vector(x, y, z);
}
 float UniformSpherePdf() {
	return 1.f / (4.f * M_PI);
}
 void UniformSampleDisk(float u1, float u2,
		float *x, float *y) {
	float r = sqrtf(u1);
	float theta = 2.0f * M_PI * u2;
	*x = r * cosf(theta);
	*y = r * sinf(theta);
}
 void ConcentricSampleDisk(float u1, float u2,
		float *dx, float *dy) {
	float r, theta;
	// Map uniform random numbers to $[-1,1]^2$
	float sx = 2 * u1 - 1;
	float sy = 2 * u2 - 1;
	// Map square to $(r,\theta)$
	// Handle degeneracy at the origin
	if (sx == 0.0 && sy == 0.0) {
		*dx = 0.0;
		*dy = 0.0;
		return;
	}
	if (sx >= -sy) {
		if (sx > sy) {
			// Handle first region of disk
			r = sx;
			if (sy > 0.0)
				theta = sy/r;
			else
				theta = 8.0f + sy/r;
		}
		else {
			// Handle second region of disk
			r = sy;
			theta = 2.0f - sx/r;
		}
	}
	else {
		if (sx <= sy) {
			// Handle third region of disk
			r = -sx;
			theta = 4.0f - sy/r;
		}
		else {
			// Handle fourth region of disk
			r = -sy;
			theta = 6.0f + sx/r;
		}
	}
	theta *= M_PI / 4.f;
	*dx = r*cosf(theta);
	*dy = r*sinf(theta);
}
 void UniformSampleTriangle(float u1, float u2,
		float *u, float *v) {
	float su1 = sqrtf(u1);
	*u = 1.f - su1;
	*v = u2 * su1;
}
 float UniformConePdf(float cosThetaMax) {
	return 1.f / (2.f * M_PI * (1.f - cosThetaMax));
}
Vector UniformSampleCone(float u1, float u2,
		float costhetamax) {
	float costheta = Lerp(u1, costhetamax, 1.f);
	float sintheta = sqrtf(1.f - costheta*costheta);
	float phi = u2 * 2.f * M_PI;
	return Vector(cosf(phi) * sintheta,
	              sinf(phi) * sintheta,
		          costheta);
}
 Vector UniformSampleCone(float u1, float u2, float costhetamax,
		const Vector &x, const Vector &y, const Vector &z) {
	float costheta = Lerp(u1, costhetamax, 1.f);
	float sintheta = sqrtf(1.f - costheta*costheta);
	float phi = u2 * 2.f * M_PI;
	return cosf(phi) * sintheta * x + sinf(phi) * sintheta * y +
		costheta * z;
}
 Vector SampleHG(const Vector &w, float g,
		float u1, float u2) {
	float costheta;
	if (fabsf(g) < 1e-3)
		costheta = 1.f - 2.f * u1;
	else {
		// NOTE - lordcrc - Bugfix, pbrt tracker id 0000082: bug in SampleHG
		float sqrTerm = (1.f - g * g) /
			(1.f - g + 2.f * g * u1);
		costheta = (1.f + g * g - sqrTerm * sqrTerm) / (2.f * g);
	}
	float sintheta = sqrtf(max(0.f, 1.f-costheta*costheta));
	float phi = 2.f * M_PI * u2;
	Vector v1, v2;
	CoordinateSystem(w, &v1, &v2);
	return SphericalDirection(sintheta, costheta,
		phi, v1, v2, w);
}
 float HGPdf(const Vector &w, const Vector &wp,
		float g) {
	return PhaseHG(w, wp, g);
}

#define  A1  (-3.969683028665376e+01)
#define  A2   2.209460984245205e+02
#define  A3  (-2.759285104469687e+02)
#define  A4   1.383577518672690e+02
#define  A5  (-3.066479806614716e+01)
#define  A6   2.506628277459239e+00

#define  B1  (-5.447609879822406e+01)
#define  B2   1.615858368580409e+02
#define  B3  (-1.556989798598866e+02)
#define  B4   6.680131188771972e+01
#define  B5  (-1.328068155288572e+01)

#define  C1  (-7.784894002430293e-03)
#define  C2  (-3.223964580411365e-01)
#define  C3  (-2.400758277161838e+00)
#define  C4  (-2.549732539343734e+00)
#define  C5   4.374664141464968e+00
#define  C6   2.938163982698783e+00

#define  D1   7.784695709041462e-03
#define  D2   3.224671290700398e-01
#define  D3   2.445134137142996e+00
#define  D4   3.754408661907416e+00

#define P_LOW   0.02425
/* P_high = 1 - p_low*/
#define P_HIGH  0.97575

double normsinv(double p)
{
	if ((0 < p )  && (p < P_LOW)) {
		const double q = sqrt(-2 * log(p));
		return (((((C1 * q + C2) * q + C3) * q + C4) * q + C5) * q + C6) /
			((((D1 * q + D2) * q + D3) * q + D4) * q + 1);
	} else if ((P_LOW <= p) && (p <= P_HIGH)) {
		const double q = p - 0.5;
		const double r = q * q;
		return (((((A1 * r + A2) * r + A3) * r + A4) * r + A5) * r + A6) * q /
			(((((B1 * r + B2) * r + B3) * r + B4) * r + B5) * r + 1);
	} else if ((P_HIGH < p) && (p < 1)) {
                   const double q = sqrt(-2 * log(1 - p));
                   return -(((((C1 * q + C2) * q + C3) * q + C4) * q + C5) * q + C6) /
			   ((((D1 * q + D2) * q + D3) * q + D4) * q + 1);
	} else {
		return 1.;
	}

/* Under UNIX OR LINUX, The return value 'x' could be corrected by the following for better accuracy.
if(( 0 < p)&&(p < 1)){
   e = 0.5 * erfc(-x/sqrt(2)) - p;
   u = e * sqrt(2*M_PI) * exp(x*x/2);
   x = x - u/(1 + x*u/2);
}
*/

}


float GaussianSampleDisk(float u1)
{
	
	float r = normsinv( u1 );
	
	return r;
}
float InverseGaussianSampleDisk(float u1)
{
	
	float r = 1.f - normsinv( u1 );
	
	return r;
}
float ExponentialSampleDisk(float u1, int power)
{
	
	float r = -log(u1)/float(power);
	
	return r;
}
float InverseExponentialSampleDisk(float u1, int power)
{
	
	float r = 1 - (-log(u1)/float(power));
	
	return r;
}
float TriangularSampleDisk(float u1)
{

	float a = 0;
	float b = 1;
	float m = 0.5;
	float r = 0;

	if( u1 <= (m-a)/(b-a))
	{
		r = a + sqrt( u1*(m-a)*(b-a) );
	}else{
			r = b - sqrt( (1-u1)*(b-m)*(b-a) );
	}	
	return r;
}

}//namespace girl

