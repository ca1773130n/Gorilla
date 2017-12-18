/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   homogeneous.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - homogeneous.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// homogeneous.cpp*
#include "homogeneous.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// HomogeneousVolume Method Definitions
VolumeRegion * HomogeneousVolume::CreateVolumeRegion(const Transform &volume2world,
		const ParamSet &params) {
	// Initialize common volume region parameters
	RGBColor sigma_a = params.FindOneRGBColor("sigma_a", 0.);
	RGBColor sigma_s = params.FindOneRGBColor("sigma_s", 0.);
	float g = params.FindOneFloat("g", 0.);
	RGBColor Le = params.FindOneRGBColor("Le", 0.);
	Point p0 = params.FindOnePoint("p0", Point(0,0,0));
	Point p1 = params.FindOnePoint("p1", Point(1,1,1));

	return new HomogeneousVolume(sigma_a, sigma_s, g, Le, BBox(p0, p1),
		volume2world);
}

static DynamicLoader::RegisterVolumeRegion<HomogeneousVolume> r("homogeneous");
