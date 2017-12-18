/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   volumegrid.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - volumegrid.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// volumegrid.cpp*
#include "volume.h"

namespace girl
{

// VolumeGrid Declarations
class GIRLCOREDLL  VolumeGrid : public DensityRegion {
public:
	// VolumeGrid Public Methods
	VolumeGrid(const RGBColor &sa, const RGBColor &ss, float gg,
	 		const RGBColor &emit, const BBox &e, const Transform &v2w,
			int nx, int ny, int nz, const float *d);
	~VolumeGrid() { delete[] density; }
	BBox WorldBound() const { return WorldToVolume.GetInverse()(extent); }
	bool IntersectP(const Ray &r, float *t0, float *t1) const {
		Ray ray = WorldToVolume(r);
		return extent.IntersectP(ray, t0, t1);
	}
	float Density(const Point &Pobj) const;
	float D(int x, int y, int z) const {
		x = Clamp(x, 0, nx-1);
		y = Clamp(y, 0, ny-1);
		z = Clamp(z, 0, nz-1);
		return density[z*nx*ny + y*nx + x];
	}
	
	static VolumeRegion *CreateVolumeRegion(const Transform &volume2world, const ParamSet &params);
private:
	// VolumeGrid Private Data
	float *density;
	const int nx, ny, nz;
	const BBox extent;
};

}//namespace girl

