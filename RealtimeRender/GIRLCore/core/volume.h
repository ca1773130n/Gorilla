/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   volume.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - volume.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_VOLUME_H
#define GIRL_VOLUME_H
// volume.h*
#include "girl.h"
#include "color.h"
#include "geometry.h"

namespace girl
{

// Volume Scattering Declarations
float PhaseIsotropic(const Vector &w, const Vector &wp);

float PhaseRayleigh(const Vector &w, const Vector &wp);

float PhaseMieHazy(const Vector &w, const Vector &wp);

float PhaseMieMurky(const Vector &w, const Vector &wp);
float PhaseHG(const Vector &w, const Vector &wp, float g);
float PhaseSchlick(const Vector &w, const Vector &wp, float g);
class GIRLCOREDLL   VolumeRegion {
public:
	// VolumeRegion Interface
	virtual ~VolumeRegion() { }
	virtual BBox WorldBound() const = 0;
	virtual bool IntersectP(const Ray &ray, float *t0,
		float *t1) const = 0;
	virtual RGBColor sigma_a(const Point &,
		const Vector &) const = 0;
	virtual RGBColor sigma_s(const Point &,
		const Vector &) const = 0;
	virtual
		RGBColor Lve(const Point &, const Vector &) const = 0;
	virtual float p(const Point &, const Vector &,
		const Vector &) const = 0;
	virtual RGBColor sigma_t(const Point &, const Vector &) const;
	virtual RGBColor Tau(const Ray &ray,
		float step = 1.f, float offset = 0.5) const = 0;
};

class GIRLCOREDLL   DensityRegion : public VolumeRegion {
public:
	// DensityRegion Public Methods
	DensityRegion(const RGBColor &sig_a, const RGBColor &sig_s,
		float g, const RGBColor &Le, const Transform &VolumeToWorld);
	virtual float Density(const Point &Pobj) const = 0;
	RGBColor sigma_a(const Point &p, const Vector &) const {
		return Density(WorldToVolume(p)) * sig_a;
	}
	RGBColor sigma_s(const Point &p, const Vector &) const {
		return Density(WorldToVolume(p)) * sig_s;
	}
	RGBColor sigma_t(const Point &p, const Vector &) const {
		return Density(WorldToVolume(p)) * (sig_a + sig_s);
	}
	RGBColor Lve(const Point &p, const Vector &) const {
		return Density(WorldToVolume(p)) * le;
	}
	float p(const Point &p, const Vector &w,
			const Vector &wp) const {
		return PhaseHG(w, wp, g);
	}
	RGBColor Tau(const Ray &r, float stepSize, float offset) const;
protected:
	// DensityRegion Protected Data
	Transform WorldToVolume;
	RGBColor sig_a, sig_s, le;
	float g;
};

class GIRLCOREDLL   AggregateVolume : public VolumeRegion {
public:
	// AggregateVolume Public Methods
	AggregateVolume(const vector<VolumeRegion *> &r);
	~AggregateVolume();
	BBox WorldBound() const;
	bool IntersectP(const Ray &ray, float *t0, float *t1) const;
	RGBColor sigma_a(const Point &, const Vector &) const;
	RGBColor sigma_s(const Point &, const Vector &) const;
	RGBColor Lve(const Point &, const Vector &) const;
	float p(const Point &, const Vector &, const Vector &) const;
	RGBColor sigma_t(const Point &, const Vector &) const;
	RGBColor Tau(const Ray &ray, float, float) const;
private:
	// AggregateVolume Private Data
	vector<VolumeRegion *> regions;
	BBox bound;
};

}//namespace girl

#endif // GIRL_VOLUME_H
