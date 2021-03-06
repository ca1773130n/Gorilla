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

#ifndef GIRL_QUATERNION_H
#define GIRL_QUATERNION_H
// quaternion.h*
#include "girl.h"
#include "geometry.h"

namespace girl
{

class GIRLCOREDLL Quaternion {

public:
	float w;
	Vector v;

	// generate quaternion from 4x4 matrix
	Quaternion(const boost::shared_ptr<Matrix4x4> m);
	Quaternion();
	Quaternion(const Quaternion &q);


	friend Quaternion operator +( const Quaternion& q1, const Quaternion& q2 ) {
		Quaternion q;
		q.v = q1.v + q2.v;
		q.w = q1.w + q2.w;
		return q;
	}

	friend Quaternion operator -( const Quaternion& q1, const Quaternion& q2 ) {
		Quaternion q;
		q.v = q1.v - q2.v;
		q.w = q1.w - q2.w;
		return q;
	}


	friend Quaternion operator *( const Quaternion& q1, const Quaternion& q2 ) {
		Quaternion q;
		q.w = q1.w*q2.w - Dot(q1.v, q2.v);
		q.v = q1.w*q2.v + q2.w*q1.v + Cross(q1.v, q2.v);
		return q;
	}

	friend Quaternion operator *( const float& f, const Quaternion& q1 ) {
		Quaternion q(q1);
		q.w = q.w * f;
		q.v = q.v * f;
		return q;
	}

	inline void Normalize() {
		float len = sqrt(w*w + Dot(v, v));
		w = w/len;
		v = v * (1.0/len);
	}
	
};

float dot(const Quaternion &q1, const Quaternion &q2);

Quaternion slerp(float t, const Quaternion &q1, const Quaternion &q2);

// get the rotation matrix from quaternion
void toMatrix(const Quaternion &q, float m[4][4]);

}//namespace girl


#endif // GIRL_QUATERNION_H
