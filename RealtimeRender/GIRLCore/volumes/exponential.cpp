/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   exponential.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - exponential.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// exponential.cpp*
#include "exponential.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// ExponentialDensity Method Definitions
VolumeRegion * ExponentialDensity::CreateVolumeRegion(const Transform &volume2world,
		const ParamSet &params) {
	// Initialize common volume region parameters
	RGBColor sigma_a = params.FindOneRGBColor("sigma_a", 0.);
	RGBColor sigma_s = params.FindOneRGBColor("sigma_s", 0.);
	float g = params.FindOneFloat("g", 0.);
	RGBColor Le = params.FindOneRGBColor("Le", 0.);
	Point p0 = params.FindOnePoint("p0", Point(0,0,0));
	Point p1 = params.FindOnePoint("p1", Point(1,1,1));
	float a = params.FindOneFloat("a", 1.);
	float b = params.FindOneFloat("b", 1.);
	Vector up = params.FindOneVector("updir", Vector(0,1,0));
	return new ExponentialDensity(sigma_a, sigma_s, g, Le, BBox(p0, p1),
		volume2world, a, b, up);
}

static DynamicLoader::RegisterVolumeRegion<ExponentialDensity> r("exponential");
