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

#ifndef GIRL_LIGHT_H
#define GIRL_LIGHT_H
// light.h*
#include "girl.h"
#include "geometry.h"
#include "spectrum.h"
#include "texture.h"
#include "primitive.h"
#include "error.h"
// Light Declarations

namespace girl
{

class  Light {
public:
	// Light Interface
	virtual ~Light();
	Light(const Transform &l2w, int ns = 1)
		: nSamples(max(1, ns)), LightToWorld(l2w),
		  WorldToLight(l2w.GetInverse()) {
		if (WorldToLight.HasScale())
			girlError(GIRL_UNIMPLEMENT,GIRL_WARNING,"Scaling detected in world-to-light transformation!\nThe system has numerous assumptions, implicit and explicit,\nthat this transform will have no scale factors in it.\nProceed at your own risk; your image may have errors or\nthe system may crash as a result of this.");
		havePortalShape = false;
		nrPortalShapes = 0;
		PortalArea = 0;
	}
	virtual SWCSpectrum Power(const TsPack *tspack, const Scene *) const = 0;
	virtual bool IsDeltaLight() const = 0;
	virtual SWCSpectrum Le(const TsPack *tspack, const RayDifferential &r) const;
	virtual SWCSpectrum Le(const TsPack *tspack, const Scene *scene, const Ray &r,
		const Normal &n, BSDF **bsdf, float *pdf, float *pdfDirect) const;
	virtual SWCSpectrum Sample_L(const TsPack *tspack, const Point &p, float u1,
		float u2, float u3, Vector *wi, float *pdf,
		VisibilityTester *vis) const = 0;
	virtual float Pdf(const Point &p,
	                  const Vector &wi) const = 0;
	virtual SWCSpectrum Sample_L(const TsPack *tspack, const Point &p, const Normal &n,
			float u1, float u2, float u3, Vector *wi, float *pdf,
			VisibilityTester *visibility) const {
		return Sample_L(tspack, p, u1, u2, u3, wi, pdf, visibility);
	}
	virtual float Pdf(const Point &p, const Normal &n,
			const Vector &wi) const {
		return Pdf(p, wi);
	}
	virtual SWCSpectrum Sample_L(const TsPack *tspack, const Scene *scene, float u1,
		float u2, float u3, float u4,
		Ray *ray, float *pdf) const = 0;
	virtual bool Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2, float u3, BSDF **bsdf, float *pdf, SWCSpectrum *Le) const {girlError(GIRL_BUG, GIRL_SEVERE, "Unimplemented Light::Sample_L"); return false;}
	virtual bool Sample_L(const TsPack *tspack, const Scene *scene, const Point &p, const Normal &n, float u1, float u2, float u3, BSDF **bsdf, float *pdf, float *pdfDirect, VisibilityTester *visibility, SWCSpectrum *Le) const {girlError(GIRL_BUG, GIRL_SEVERE, "Unimplemented Light::Sample_L"); return false;}

	void AddPortalShape(boost::shared_ptr<Primitive> shape);

	// Light Public Data
	const int nSamples;
	bool havePortalShape;
	int nrPortalShapes;
	vector<boost::shared_ptr<Primitive> > PortalShapes;
	float PortalArea;
protected:
	// Light Protected Data
	const Transform LightToWorld, WorldToLight;
};

struct VisibilityTester {
	// VisibilityTester Public Methods

	void SetSegment(const Point &p1, const Point & p2, float time) {
		// Dade - need to scale the RAY_EPSILON value because the ray direction
		// is not normalized (in order to avoid light leaks: bug #295)
		Vector w = p2 - p1;
		float epsilon = SHADOW_RAY_EPSILON / w.Length();
		r = Ray(p1, w, epsilon, 1.f - epsilon);
		r.time = time;
	}

	void SetRay(const Point &p, const Vector & w, float time) {
		// Dade - need to scale the RAY_EPSILON value because the ray direction
		// is not normalized (in order to avoid light leaks: bug #295)
		float epsilon = SHADOW_RAY_EPSILON / w.Length();
		r = Ray(p, w, epsilon);
		r.time = time;
	}

	bool Unoccluded(const Scene * scene) const;
	bool TestOcclusion(const TsPack *tspack, const Scene *scene, SWCSpectrum * f) const;
	// modulates the supplied SWCSpectrum with the transmittance along the ray
	void Transmittance(const TsPack *tspack, const Scene * scene, const Sample *sample, SWCSpectrum *const L) const;
	Ray r;
};

class AreaLight : public Light {
public:
	// AreaLight Interface
	AreaLight(const Transform &light2world,
		boost::shared_ptr<Texture<SWCSpectrum> > Le, float g, float pow, float e, int ns, const boost::shared_ptr<Primitive> &prim);
	~AreaLight();
	virtual SWCSpectrum L(const TsPack *tspack, const Point &p, const Normal &n,
			const Vector &w) const {
		Vector dpdu, dpdv;
		CoordinateSystem(Vector(n), &dpdu, &dpdv);
		DifferentialGeometry dg(p, n, dpdu, dpdv, Vector(0, 0, 0), Vector(0, 0, 0), 0, 0, NULL);
		// TODO - radiance - add shading geometry for uv coordinates

		return Dot(n, w) > 0 ? Le->Evaluate(tspack, dg) * gain : 0.;
	}
	virtual SWCSpectrum L(const TsPack *tspack, const Ray &ray, const DifferentialGeometry &dg, const Normal &n, BSDF **bsdf, float *pdf, float *pdfDirect) const;
	SWCSpectrum Power(const TsPack *tspack, const Scene *) const {
		return Le->Evaluate(tspack, dummydg) * gain * area * M_PI;
	}
	bool IsDeltaLight() const { return false; }
	float Pdf(const Point &, const Vector &) const;
	float Pdf(const Point &, const Normal &, const Vector &) const;
	virtual SWCSpectrum Sample_L(const TsPack *tspack, const Point &P, const Normal &N,
		float u1, float u2, float u3, Vector *wo, float *pdf,
		VisibilityTester *visibility) const;
	virtual SWCSpectrum Sample_L(const TsPack *tspack, const Point &P, float u1, float u2, float u3,
		Vector *wo, float *pdf, VisibilityTester *visibility) const;
	SWCSpectrum Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2,
			float u3, float u4, Ray *ray, float *pdf) const;
	virtual bool Sample_L(const TsPack *tspack, const Scene *scene, float u1, float u2, float u3, BSDF **bsdf, float *pdf, SWCSpectrum *Le) const;
	virtual bool Sample_L(const TsPack *tspack, const Scene *scene, const Point &p, const Normal &n, float u1, float u2, float u3, BSDF **bsdf, float *pdf, float *pdfDirect, VisibilityTester *visibility, SWCSpectrum *Le) const;
	static AreaLight *CreateAreaLight(const Transform &light2world, const ParamSet &paramSet, const TextureParams &tp,
		const boost::shared_ptr<Primitive> &prim);
protected:
	// AreaLight Protected Data
	boost::shared_ptr<Texture<SWCSpectrum> > Le;
	DifferentialGeometry dummydg;
	boost::shared_ptr<Primitive> prim;
	float gain, power, efficacy, area;
};

}//namespace girl

#endif // GIRL_LIGHT_H
