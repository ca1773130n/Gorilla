/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   raydifferential.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - raydifferential.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////


#ifndef GIRL_RAYDIFFERENTIAL_H
#define GIRL_RAYDIFFERENTIAL_H

#include "vector.h"
#include "point.h"
#include "ray.h"

namespace girl
{

class GIRLCOREDLL   RayDifferential : public Ray {
public:
	// RayDifferential Methods
	RayDifferential() { hasDifferentials = false; }
	RayDifferential(const Point &org, const Vector &dir)
			: Ray(org, dir) {
		hasDifferentials = false;
	}
	explicit RayDifferential(const Ray &ray) : Ray(ray) {
		hasDifferentials = false;
	}
	// RayDifferential Public Data
	
	Ray rx, ry;
	bool hasDifferentials;
	

};

}//namespace girl

#endif //GIRL_RAYDIFFERENTIAL_H
