/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   lambertian.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - lambertian.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// lambertian.cpp*
#include "lambertian.h"
#include "spectrum.h"
#include "mc.h"

using namespace girl;

void Lambertian::f(const TsPack *tspack, const Vector &wo, 
				   const Vector &wi, SWCSpectrum *const f) const {
	*f += RoverPI;
}

