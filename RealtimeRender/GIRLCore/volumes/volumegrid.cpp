/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   volumegrid.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - volumegrid.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// volumegrid.cpp*
#include "volumegrid.h"
#include "paramset.h"
#include "dynload.h"
#include "error.h"

using namespace girl;

// VolumeGrid Method Definitions
VolumeGrid::VolumeGrid(const RGBColor &sa,
		const RGBColor &ss, float gg,
 		const RGBColor &emit, const BBox &e,
		const Transform &v2w,
		int x, int y, int z, const float *d)
	: DensityRegion(sa, ss, gg, emit, v2w),
	nx(x), ny(y), nz(z), extent(e) {
	density = new float[nx*ny*nz];
	memcpy(density, d, nx*ny*nz*sizeof(float));
}
float VolumeGrid::Density(const Point &Pobj) const {
	if (!extent.Inside(Pobj)) return 0;
	// Compute voxel coordinates and offsets for _Pobj_
	float voxx = (Pobj.x - extent.pMin.x) /
		(extent.pMax.x - extent.pMin.x) * nx - .5f;
	float voxy = (Pobj.y - extent.pMin.y) /
		(extent.pMax.y - extent.pMin.y) * ny - .5f;
	float voxz = (Pobj.z - extent.pMin.z) /
		(extent.pMax.z - extent.pMin.z) * nz - .5f;
	int vx = Floor2Int(voxx);
	int vy = Floor2Int(voxy);
	int vz = Floor2Int(voxz);
	float dx = voxx - vx, dy = voxy - vy, dz = voxz - vz;
	// Trilinearly interpolate density values to compute local density
	float d00 = Lerp(dx, D(vx, vy, vz),     D(vx+1, vy, vz));
	float d10 = Lerp(dx, D(vx, vy+1, vz),  D(vx+1, vy+1, vz));
	float d01 = Lerp(dx, D(vx, vy, vz+1),  D(vx+1, vy, vz+1));
	float d11 = Lerp(dx, D(vx, vy+1, vz+1),D(vx+1, vy+1, vz+1));
	float d0 = Lerp(dy, d00, d10);
	float d1 = Lerp(dy, d01, d11);
	return Lerp(dz, d0, d1);
}
VolumeRegion * VolumeGrid::CreateVolumeRegion(const Transform &volume2world,
		const ParamSet &params) {
	// Initialize common volume region parameters
	RGBColor sigma_a = params.FindOneRGBColor("sigma_a", 0.);
	RGBColor sigma_s = params.FindOneRGBColor("sigma_s", 0.);
	float g = params.FindOneFloat("g", 0.);
	RGBColor Le = params.FindOneRGBColor("Le", 0.);
	Point p0 = params.FindOnePoint("p0", Point(0,0,0));
	Point p1 = params.FindOnePoint("p1", Point(1,1,1));
	int nitems;
	const float *data = params.FindFloat("density", &nitems);
	if (!data) {
		girlError(GIRL_MISSINGDATA,GIRL_ERROR,"No \"density\" values provided for volume grid?");
		return NULL;
	}
	int nx = params.FindOneInt("nx", 1);
	int ny = params.FindOneInt("ny", 1);
	int nz = params.FindOneInt("nz", 1);
	if (nitems != nx*ny*nz) {
		//Error("VolumeGrid has %d density values but nx*ny*nz = %d",
		//	nitems, nx*ny*nz);
		std::stringstream ss;
		ss<<"VolumeGrid has "<<nitems<<" density values but nx*ny*nz = "<<nx*ny*nz;
		girlError(GIRL_CONSISTENCY,GIRL_ERROR,ss.str().c_str());
		return NULL;
	}
	return new VolumeGrid(sigma_a, sigma_s, g, Le, BBox(p0, p1),
		volume2world, nx, ny, nz, data);
}

static DynamicLoader::RegisterVolumeRegion<VolumeGrid> r("volumegrid");
