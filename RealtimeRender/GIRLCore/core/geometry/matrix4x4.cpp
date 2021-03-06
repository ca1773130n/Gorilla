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

// matrix4x4.cpp*
#include "girl.h"

//#ifndef GIRL_USE_SSE
#include "matrix4x4.h"
#include "error.h"

namespace girl
{

Matrix4x4::Matrix4x4(float mat[4][4])
{
	memcpy(m, mat, 16 * sizeof(float));
}

Matrix4x4::Matrix4x4(float t00, float t01, float t02, float t03,
                     float t10, float t11, float t12, float t13,
                     float t20, float t21, float t22, float t23,
                     float t30, float t31, float t32, float t33)
{
	m[0][0] = t00; m[0][1] = t01; m[0][2] = t02; m[0][3] = t03;
	m[1][0] = t10; m[1][1] = t11; m[1][2] = t12; m[1][3] = t13;
	m[2][0] = t20; m[2][1] = t21; m[2][2] = t22; m[2][3] = t23;
	m[3][0] = t30; m[3][1] = t31; m[3][2] = t32; m[3][3] = t33;
}

boost::shared_ptr<Matrix4x4> Matrix4x4::Transpose() const
{
	boost::shared_ptr<Matrix4x4> o(new Matrix4x4(m[0][0], m[1][0], m[2][0], m[3][0],
		m[0][1], m[1][1], m[2][1], m[3][1],
		m[0][2], m[1][2], m[2][2], m[3][2],
		m[0][3], m[1][3], m[2][3], m[3][3]));
	return o;
}

boost::shared_ptr<Matrix4x4> Matrix4x4::Inverse() const
{
	int indxc[4], indxr[4];
	int ipiv[4] = { 0, 0, 0, 0 };
	float minv[4][4];
	memcpy(minv, m, 4 * 4 * sizeof(float));
	for (int i = 0; i < 4; ++i) {
		int irow = -1, icol = -1;
		float big = 0.;
		// Choose pivot
		for (int j = 0; j < 4; ++j) {
			if (ipiv[j] != 1) {
				for (int k = 0; k < 4; ++k) {
					if (ipiv[k] == 0) {
						if (fabsf(minv[j][k]) >= big) {
							big = fabsf(minv[j][k]);
							irow = j;
							icol = k;
						}
					} else if (ipiv[k] > 1)
						girlError(GIRL_MATH, GIRL_ERROR, "Singular matrix in MatrixInvert");
				}
			}
		}
		++ipiv[icol];
		// Swap rows _irow_ and _icol_ for pivot
		if (irow != icol) {
			for (int k = 0; k < 4; ++k)
				swap(minv[irow][k], minv[icol][k]);
		}
		indxr[i] = irow;
		indxc[i] = icol;
		if (minv[icol][icol] == 0.)
			girlError(GIRL_MATH, GIRL_ERROR, "Singular matrix in MatrixInvert");
		// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
		float pivinv = 1.f / minv[icol][icol];
		minv[icol][icol] = 1.f;
		for (int j = 0; j < 4; ++j)
			minv[icol][j] *= pivinv;
		// Subtract this row from others to zero out their columns
		for (int j = 0; j < 4; ++j) {
			if (j != icol) {
				float save = minv[j][icol];
				minv[j][icol] = 0;
				for (int k = 0; k < 4; ++k)
					minv[j][k] -= minv[icol][k] * save;
			}
		}
	}
	// Swap columns to reflect permutation
	for (int j = 3; j >= 0; --j) {
		if (indxr[j] != indxc[j]) {
			for (int k = 0; k < 4; ++k)
				swap(minv[k][indxr[j]], minv[k][indxc[j]]);
		}
	}
	//return new Matrix4x4(minv);
	boost::shared_ptr<Matrix4x4> o(new Matrix4x4(minv));
	return o;
}

}

/*#else // GIRL_USE_SSE

#include "matrix4x4-sse.h"

typedef long int32; //!Jeanphi: replaced int by long. Should use POSIX types
#ifdef __GNUC__
const float __attribute__ ((aligned(16))) _F1001[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
const int32 __attribute__ ((aligned(16))) _Sign_PNNP[4] = { 0x00000000, 0x80000000, 0x80000000, 0x00000000 };
#else //intel compiler
const _MM_ALIGN16 float _F1001[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
const _MM_ALIGN16 __int32 _Sign_PNNP[4] = { 0x00000000, 0x80000000, 0x80000000, 0x00000000 };
#endif
#define Sign_PNNP   (*(__m128*)&_Sign_PNNP)        // + - - +

namespace girl
{

Matrix4x4::Matrix4x4(float mat[4][4])
{
	_L1 = _mm_set_ps(mat[3][0], mat[2][0], mat[1][0], mat[0][0]);
	_L2 = _mm_set_ps(mat[3][1], mat[2][1], mat[1][1], mat[0][1]);
	_L3 = _mm_set_ps(mat[3][2], mat[2][2], mat[1][2], mat[0][2]);
	_L4 = _mm_set_ps(mat[3][3], mat[2][3], mat[1][3], mat[0][3]);
}

Matrix4x4::Matrix4x4(float f11, float f12, float f13, float f14,
               float f21, float f22, float f23, float f24,
               float f31, float f32, float f33, float f34,
               float f41, float f42, float f43, float f44)
{
	_L1 = _mm_set_ps(f41, f31, f21, f11);
	_L2 = _mm_set_ps(f42, f32, f22, f12);
	_L3 = _mm_set_ps(f43, f33, f23, f13);
	_L4 = _mm_set_ps(f44, f34, f24, f14);
}

boost::shared_ptr<Matrix4x4> Matrix4x4::Transpose() const
{
	__m128 xmm0 = _mm_unpacklo_ps(_L1, _L2),
		xmm1 = _mm_unpacklo_ps(_L3, _L4),
		xmm2 = _mm_unpackhi_ps(_L1, _L2),
		xmm3 = _mm_unpackhi_ps(_L3, _L4);

	__m128 L1, L2, L3, L4;
	L1 = _mm_movelh_ps(xmm0, xmm1);
	L2 = _mm_movehl_ps(xmm1, xmm0);
	L3 = _mm_movelh_ps(xmm2, xmm3);
	L4 = _mm_movehl_ps(xmm3, xmm2);

	boost::shared_ptr<Matrix4x4> o(new Matrix4x4(L1, L2, L3, L4));
	return o;
}

boost::shared_ptr<Matrix4x4> Matrix4x4::Inverse() const
{
	__m128 A = _mm_movelh_ps(_L1, _L2),	// the four sub-matrices
		B = _mm_movehl_ps(_L2, _L1),
		C = _mm_movelh_ps(_L3, _L4),
		D = _mm_movehl_ps(_L4, _L3);
	__m128 iA, iB, iC, iD,	// partial inverse of the sub-matrices
		DC, AB;
	__m128 dA, dB, dC, dD;	// determinant of the sub-matrices
	__m128 det, d, d1, d2;
	__m128 rd;

	//  AB = A# * B
	AB = _mm_mul_ps(_mm_shuffle_ps(A, A, 0x0F), B);
	AB = _mm_sub_ps(AB, _mm_mul_ps(_mm_shuffle_ps(A, A, 0xA5),
		_mm_shuffle_ps(B, B, 0x4E)));
	//  DC = D# * C
	DC = _mm_mul_ps(_mm_shuffle_ps(D, D, 0x0F), C);
	DC = _mm_sub_ps(DC, _mm_mul_ps(_mm_shuffle_ps(D, D, 0xA5),
		_mm_shuffle_ps(C, C, 0x4E)));

	//  dA = |A|
	dA = _mm_mul_ps(_mm_shuffle_ps(A, A, 0x5F), A);
	dA = _mm_sub_ss(dA, _mm_movehl_ps(dA, dA));
	//  dB = |B|
	dB = _mm_mul_ps(_mm_shuffle_ps(B, B, 0x5F), B);
	dB = _mm_sub_ss(dB, _mm_movehl_ps(dB, dB));

	//  dC = |C|
	dC = _mm_mul_ps(_mm_shuffle_ps(C, C, 0x5F), C);
	dC = _mm_sub_ss(dC, _mm_movehl_ps(dC, dC));
	//  dD = |D|
	dD = _mm_mul_ps(_mm_shuffle_ps(D, D, 0x5F), D);
	dD = _mm_sub_ss(dD, _mm_movehl_ps(dD, dD));

	//  d = trace(AB*DC) = trace(A#*B*D#*C)
	d = _mm_mul_ps(_mm_shuffle_ps(DC, DC, 0xD8), AB);

	//  iD = C*A#*B
	iD = _mm_mul_ps(_mm_shuffle_ps(C, C, 0xA0), _mm_movelh_ps(AB, AB));
	iD = _mm_add_ps(iD, _mm_mul_ps(_mm_shuffle_ps(C, C, 0xF5),
		_mm_movehl_ps(AB, AB)));
	//  iA = B*D#*C
	iA = _mm_mul_ps(_mm_shuffle_ps(B, B, 0xA0), _mm_movelh_ps(DC, DC));
	iA = _mm_add_ps(iA, _mm_mul_ps(_mm_shuffle_ps(B, B, 0xF5),
		_mm_movehl_ps(DC, DC)));

	//  d = trace(AB*DC) = trace(A#*B*D#*C) [continue]
	d = _mm_add_ps(d, _mm_movehl_ps(d, d));
	d = _mm_add_ss(d, _mm_shuffle_ps(d, d, 1));
	d1 = _mm_mul_ps(dA, dD);
	d2 = _mm_mul_ps(dB, dC);

	//  iD = D*|A| - C*A#*B
	iD = _mm_sub_ps(_mm_mul_ps(D, _mm_shuffle_ps(dA, dA, 0)), iD);

	//  iA = A*|D| - B*D#*C;
	iA = _mm_sub_ps(_mm_mul_ps(A, _mm_shuffle_ps(dD, dD, 0)), iA);

	//  det = |A|*|D| + |B|*|C| - trace(A#*B*D#*C)
	det = _mm_sub_ps(_mm_add_ps(d1, d2), d);
	rd = (__m128)(_mm_div_ps(_mm_set_ss(1.0f), det));
#ifdef ZERO_SINGULAR
	rd = _mm_and_ps(_mm_cmpneq_ss(det, _mm_setzero_ps()), rd);
#endif

	//  iB = D * (A#B)# = D*B#*A
	iB = _mm_mul_ps(D, _mm_shuffle_ps(AB, AB, 0x33));
	iB = _mm_sub_ps(iB, _mm_mul_ps(_mm_shuffle_ps(D, D, 0xB1),
		_mm_shuffle_ps(AB, AB, 0x66)));
	//  iC = A * (D#C)# = A*C#*D
	iC = _mm_mul_ps(A, _mm_shuffle_ps(DC, DC, 0x33));
	iC = _mm_sub_ps(iC, _mm_mul_ps(_mm_shuffle_ps(A, A, 0xB1),
		_mm_shuffle_ps(DC, DC, 0x66)));

	rd = _mm_shuffle_ps(rd, rd, 0);
	rd = _mm_xor_ps(rd, Sign_PNNP);

	//  iB = C*|B| - D*B#*A
	iB = _mm_sub_ps(_mm_mul_ps(C, _mm_shuffle_ps(dB, dB, 0)), iB);

	//  iC = B*|C| - A*C#*D;
	iC = _mm_sub_ps(_mm_mul_ps(B, _mm_shuffle_ps(dC, dC, 0)) ,iC);

	//  iX = iX / det
	iA =_mm_mul_ps(iA, rd);
	iB =_mm_mul_ps(iB, rd);
	iC =_mm_mul_ps(iC, rd);
	iD =_mm_mul_ps(iD, rd);

	//return new Matrix4x4(_mm_shuffle_ps(iA,iB,0x77),_mm_shuffle_ps(iA,iB,0x22),_mm_shuffle_ps(iC,iD,0x77),_mm_shuffle_ps(iC,iD,0x22));

	boost::shared_ptr<Matrix4x4>
		o(new Matrix4x4(_mm_shuffle_ps(iA, iB, 0x77),
			_mm_shuffle_ps(iA, iB, 0x22),
			_mm_shuffle_ps(iC, iD, 0x77),
			_mm_shuffle_ps(iC, iD, 0x22)));
	return o;
}

}

#endif // GIRL_USE_SSE */



