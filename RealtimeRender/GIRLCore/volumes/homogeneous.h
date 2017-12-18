/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   homogeneous.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - homogeneous.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// homogeneous.cpp*
#include "volume.h"

namespace girl
{

// HomogeneousVolume Declarations
class GIRLCOREDLL  HomogeneousVolume : public VolumeRegion {
public:
	// HomogeneousVolume Public Methods
	HomogeneousVolume(const RGBColor &sa, const RGBColor &ss, float gg,
		 	const RGBColor &emit, const BBox &e,
			const Transform &v2w) {
		WorldToVolume = v2w.GetInverse();
		sig_a = sa;
		sig_s = ss;
		g = gg;
		le = emit;
		extent = e;
	}
	BBox WorldBound() const {
		return WorldToVolume.GetInverse()(extent);
	}
	bool IntersectP(const Ray &r, float *t0, float *t1) const {
		Ray ray = WorldToVolume(r);
		return extent.IntersectP(ray, t0, t1);
	}
	RGBColor sigma_a(const Point &p, const Vector &) const {
		return extent.Inside(WorldToVolume(p)) ? sig_a : 0.;
	}
	RGBColor sigma_s(const Point &p, const Vector &) const {
		return extent.Inside(WorldToVolume(p)) ? sig_s : 0.;
	}
	RGBColor sigma_t(const Point &p, const Vector &) const {
		return extent.Inside(WorldToVolume(p)) ? (sig_a + sig_s) : RGBColor(0.f);
	}
	RGBColor Lve(const Point &p, const Vector &) const {
		return extent.Inside(WorldToVolume(p)) ? le : 0.;
	}
	float p(const Point &p, const Vector &wi, const Vector &wo) const {
		if (!extent.Inside(WorldToVolume(p))) return 0.;
		return PhaseHG(wi, wo, g);
	}
	RGBColor Tau(const Ray &ray, float, float) const {
		float t0, t1;
		if (!IntersectP(ray, &t0, &t1)) return 0.;

		return Distance(ray(t0), ray(t1)) * (sig_a + sig_s);
	}
	
	static VolumeRegion *CreateVolumeRegion(const Transform &volume2world, const ParamSet &params);
private:
	// HomogeneousVolume Private Data
	RGBColor sig_a, sig_s, le;
	float g;
	BBox extent;
	Transform WorldToVolume;
};

}//namespace girl
