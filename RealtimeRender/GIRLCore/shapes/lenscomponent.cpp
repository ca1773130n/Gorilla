/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   lenscomponent.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - lenscomponent.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

/*
* lenscomponent for realistic camera, based on:
* Lens Component plugin Copyright(c) 2004 Nico Galoppo von Borries
*/

// lenscomponent.cpp*
#include "lenscomponent.h"
#include "mc.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// LensComponent Method Definitions
LensComponent::LensComponent(const Transform &o2w, bool ro, float rad,
               float z0, float z1, float pm, float ap)
               : Shape(o2w, ro) {
                   radius = rad;
                   zmin = Clamp(min(z0, z1), -radius, radius);
                   zmax = Clamp(max(z0, z1), -radius, radius);
                   thetaMin = acosf(Clamp(zmin/radius, -1.f, 1.f));
                   thetaMax = acosf(Clamp(zmax/radius, -1.f, 1.f));
                   phiMax = Radians(Clamp(pm, 0.0f, 360.0f));
                   apRadius = ap / 2.0f;
}
Point LensComponent::Sample(float u1, float u2, float u3, Normal *n) const {
    float lensU, lensV;
    ConcentricSampleDisk(u1, u2, &lensU, &lensV);
    lensU *= radius;
    lensV *= radius;
    return ObjectToWorld(Point(0,0,0)+Vector(lensU, lensV, 0.f));
}
BBox LensComponent::ObjectBound() const {
return BBox(Point(-radius, -radius, zmin),
    Point(radius,  radius, zmax));
}
bool LensComponent::Intersect(const Ray &r, float *tHit,
DifferentialGeometry *dg) const {
    float phi;
    Point phit;
    // Transform _Ray_ to object space
    Ray ray;
    WorldToObject(r, &ray);
    // Compute quadratic LensComponent coefficients
    float A = ray.d.x*ray.d.x + ray.d.y*ray.d.y +
        ray.d.z*ray.d.z;
    float B = 2 * (ray.d.x*ray.o.x + ray.d.y*ray.o.y +
        ray.d.z*ray.o.z);
    float C = ray.o.x*ray.o.x + ray.o.y*ray.o.y +
        ray.o.z*ray.o.z - radius*radius;
    // Solve quadratic equation for _t_ values
    float t0, t1;
    if (!Quadratic(A, B, C, &t0, &t1))
        return false;
    // Compute intersection distance along ray
    if (t0 > ray.maxt || t1 < ray.mint)
        return false;
    float thit = t0;
    if (t0 < ray.mint) {
        thit = t1;
        if (thit > ray.maxt) return false;
    }
    // Compute LensComponent hit position and $\phi$
    phit = ray(thit);
    phi = atan2f(phit.y, phit.x);
    if (phi < 0.) phi += 2.f*M_PI;
    // Test LensComponent aperture
    float dist2 = phit.x * phit.x + phit.y * phit.y;
    if (dist2 > apRadius * apRadius) return false;
    // Find parametric representation of LensComponent hit
    float u = phi / phiMax;
    float theta = acosf(phit.z / radius);
    float v = (theta - thetaMin) / (thetaMax - thetaMin);
    // Compute LensComponent \dpdu and \dpdv
    float zradius = sqrtf(phit.x*phit.x + phit.y*phit.y);
    float invzradius = 1.f / zradius;
    float cosphi = phit.x * invzradius;
    float sinphi = phit.y * invzradius;
    Vector dpdu(-phiMax * phit.y, phiMax * phit.x, 0);
    Vector dpdv = (thetaMax-thetaMin) *
        Vector(phit.z * cosphi, phit.z * sinphi,
        -radius * sinf(theta));
    //// Compute LensComponent \dndu and \dndv
    //Vector d2Pduu = -phiMax * phiMax * Vector(phit.x,phit.y,0);
    //Vector d2Pduv = (thetaMax - thetaMin) *
    //    phit.z * phiMax *
    //    Vector(-sinphi, cosphi, 0.);
    //Vector d2Pdvv = -(thetaMax - thetaMin) *
    //    (thetaMax - thetaMin) *
    //    Vector(phit.x, phit.y, phit.z);
    //// Compute coefficients for fundamental forms
    //float E = Dot(dpdu, dpdu);
    //float F = Dot(dpdu, dpdv);
    //float G = Dot(dpdv, dpdv);
    //Vector N = Normalize(Cross(dpdu, dpdv));
    //float e = Dot(N, d2Pduu);
    //float f = Dot(N, d2Pduv);
    //float g = Dot(N, d2Pdvv);
    //// Compute \dndu and \dndv from fundamental form coefficients
    //float invEGF2 = 1.f / (E*G - F*F);
    //Vector dndu = (f*F - e*G) * invEGF2 * dpdu +
    //    (e*F - f*E) * invEGF2 * dpdv;
    //Vector dndv = (g*F - f*G) * invEGF2 * dpdu +
    //    (f*F - g*E) * invEGF2 * dpdv;
    // Initialize _DifferentialGeometry_ from parametric information
    *dg = DifferentialGeometry(ObjectToWorld(phit),
        ObjectToWorld(dpdu),
        ObjectToWorld(dpdv),
        ObjectToWorld(Vector()),
        ObjectToWorld(Vector()),
        u, v, this);
    // Update _tHit_ for quadric intersection
    *tHit = thit;
    return true;
}
bool LensComponent::IntersectP(const Ray &r) const {
    //float phi;
    Point phit;
    // Transform _Ray_ to object space
    Ray ray;
    WorldToObject(r, &ray);
    // Compute quadratic LensComponent coefficients
    float A = ray.d.x*ray.d.x + ray.d.y*ray.d.y +
        ray.d.z*ray.d.z;
    float B = 2 * (ray.d.x*ray.o.x + ray.d.y*ray.o.y +
        ray.d.z*ray.o.z);
    float C = ray.o.x*ray.o.x + ray.o.y*ray.o.y +
        ray.o.z*ray.o.z - radius*radius;
    // Solve quadratic equation for _t_ values
    float t0, t1;
    if (!Quadratic(A, B, C, &t0, &t1))
        return false;
    // Compute intersection distance along ray
    if (t0 > ray.maxt || t1 < ray.mint)
        return false;
    float thit = t0;
    if (t0 < ray.mint) {
        thit = t1;
        if (thit > ray.maxt) return false;
    }
    // Compute LensComponent hit position and $\phi$
    phit = ray(thit);
    //phi = atan2f(phit.y, phit.x);
    //if (phi < 0.) phi += 2.f*M_PI;
    // Test LensComponent aperture
    float dist2 = phit.x * phit.x + phit.y * phit.y;
    if (dist2 > apRadius * apRadius) return false;
    // Test LensComponent intersection against clipping parameters
    //if (phit.z < zmin || phit.z > zmax || phi > phiMax) {
    //    if (thit == t1) return false;
    //    if (t1 > ray.maxt) return false;
    //    thit = t1;
    //    // Compute LensComponent hit position and $\phi$
    //    phit = ray(thit);
    //    phi = atan2f(phit.y, phit.x);
    //    if (phi < 0.) phi += 2.f*M_PI;
    //    if (phit.z < zmin || phit.z > zmax || phi > phiMax)
    //        return false;
    //}
    return true;
}
float LensComponent::Area() const {
    return phiMax * radius * (zmax-zmin);
}

Shape* LensComponent::CreateShape(const Transform &o2w,
                   bool reverseOrientation,
                   const ParamSet &params) {
    float radius = params.FindOneFloat("radius", 1.f);
    float zmin = params.FindOneFloat("zmin", -radius);
    float zmax = params.FindOneFloat("zmax", radius);
    float phimax = params.FindOneFloat("phimax", 360.f);
    float aperture = params.FindOneFloat("aperture", 1.f);
    return new LensComponent(o2w, reverseOrientation, radius,
        zmin, zmax, phimax, aperture);
}

static DynamicLoader::RegisterShape<LensComponent> r("lenscomponent");

