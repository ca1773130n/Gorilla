/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   exponential.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - exponential.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// exponential.cpp*
#include "volume.h"

namespace girl
{

// ExponentialDensity Declarations
class GIRLCOREDLL  ExponentialDensity : public DensityRegion {
public:
	// ExponentialDensity Public Methods
	ExponentialDensity(const RGBColor &sa, const RGBColor &ss,
			float gg, const RGBColor &emit, const BBox &e,
			const Transform &v2w, float aa, float bb,
			const Vector &up)
		: DensityRegion(sa, ss, gg, emit, v2w),
		  extent(e), a(aa), b(bb) {
		upDir = Normalize(up);
	}
	BBox WorldBound() const { return WorldToVolume.GetInverse()(extent); }
	bool IntersectP(const Ray &r, float *t0, float *t1) const {
		Ray ray = WorldToVolume(r);
		return extent.IntersectP(ray, t0, t1);
	}
	float Density(const Point &Pobj) const {
		if (!extent.Inside(Pobj)) return 0;
		float height = Dot(Pobj - extent.pMin, upDir);
		return a * expf(-b * height);
	}
	
	static VolumeRegion *CreateVolumeRegion(const Transform &volume2world, const ParamSet &params);
private:
	// ExponentialDensity Private Data
	BBox extent;
	float a, b;
	Vector upDir;
};

}//namespace girl

