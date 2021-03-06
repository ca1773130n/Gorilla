/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   meshbarytriangle.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - meshbarytriangle.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////


#include "mesh.h"
#include "mc.h"

using namespace girl;

BBox MeshBaryTriangle::ObjectBound() const {
    // Get triangle vertices in _p1_, _p2_, and _p3_
    const Point &p1 = mesh->p[v[0]];
    const Point &p2 = mesh->p[v[1]];
    const Point &p3 = mesh->p[v[2]];
    return Union(BBox(mesh->WorldToObject(p1), mesh->WorldToObject(p2)),
    		mesh->WorldToObject(p3));
}

BBox MeshBaryTriangle::WorldBound() const {
    // Get triangle vertices in _p1_, _p2_, and _p3_
    const Point &p1 = mesh->p[v[0]];
    const Point &p2 = mesh->p[v[1]];
    const Point &p3 = mesh->p[v[2]];
    return Union(BBox(p1, p2), p3);
}

bool MeshBaryTriangle::Intersect(const Ray &ray, Intersection* isect) const {
    Vector e1, e2, s1;
    // Compute $\VEC{s}_1$
    // Get triangle vertices in _p1_, _p2_, and _p3_
    const Point &p1 = mesh->p[v[0]];
    const Point &p2 = mesh->p[v[1]];
    const Point &p3 = mesh->p[v[2]];
    e1 = p2 - p1;
    e2 = p3 - p1;
    s1 = Cross(ray.d, e2);
    const float divisor = Dot(s1, e1);
    if (divisor == 0.f)
        return false;
    const float invDivisor = 1.f / divisor;
    // Compute first barycentric coordinate
    const Vector d = ray.o - p1;
    const float b1 = Dot(d, s1) * invDivisor;
    if (b1 < 0.f || b1 > 1.f)
        return false;
    // Compute second barycentric coordinate
    const Vector s2 = Cross(d, e1);
    const float b2 = Dot(ray.d, s2) * invDivisor;
    if (b2 < 0.f || b1 + b2 > 1.f)
        return false;
    // Compute _t_ to intersection point
    const float t = Dot(e2, s2) * invDivisor;
    if (t < ray.mint || t > ray.maxt)
        return false;

	// Fill in _DifferentialGeometry_ from triangle hit
    // Compute triangle partial derivatives
    Vector dpdu, dpdv;
    float uvs[3][2];
    GetUVs(uvs);
    // Compute deltas for triangle partial derivatives
    const float du1 = uvs[0][0] - uvs[2][0];
    const float du2 = uvs[1][0] - uvs[2][0];
    const float dv1 = uvs[0][1] - uvs[2][1];
    const float dv2 = uvs[1][1] - uvs[2][1];
    const Vector dp1 = p1 - p3, dp2 = p2 - p3;
    const float determinant = du1 * dv2 - dv1 * du2;
    if (determinant == 0.f) {
        // Handle zero determinant for triangle partial derivative matrix
        CoordinateSystem(Normalize(Cross(e1, e2)), &dpdu, &dpdv);
    }
    else {
        const float invdet = 1.f / determinant;
        dpdu = ( dv2 * dp1 - dv1 * dp2) * invdet;
        dpdv = (-du2 * dp1 + du1 * dp2) * invdet;
    }

    // Interpolate $(u,v)$ triangle parametric coordinates
    const float b0 = 1 - b1 - b2;
    const float tu = b0*uvs[0][0] + b1*uvs[1][0] + b2*uvs[2][0];
    const float tv = b0*uvs[0][1] + b1*uvs[1][1] + b2*uvs[2][1];

	// Dade - using the intepolated normal here in order to fix bug #340
	Normal nn;
	if (mesh->n)
		nn = Normalize(mesh->ObjectToWorld(b0 * mesh->n[v[0]] +
			b1 * mesh->n[v[1]] + b2 * mesh->n[v[2]]));
	else
		nn = Normal(Normalize(Cross(e1, e2)));

    isect->dg = DifferentialGeometry(ray(t),
			nn,
			dpdu, dpdv,
            Vector(0, 0, 0), Vector(0, 0, 0),
            tu, tv, this);
    isect->dg.AdjustNormal(mesh->reverseOrientation, mesh->transformSwapsHandedness);

    isect->Set(mesh->WorldToObject, this, mesh->GetMaterial().get());
	ray.maxt = t;

    return true;
}

bool MeshBaryTriangle::IntersectP(const Ray &ray) const {
    // Compute $\VEC{s}_1$
    // Get triangle vertices in _p1_, _p2_, and _p3_
    const Point &p1 = mesh->p[v[0]];
    const Point &p2 = mesh->p[v[1]];
    const Point &p3 = mesh->p[v[2]];
    Vector e1 = p2 - p1;
    Vector e2 = p3 - p1;
    Vector s1 = Cross(ray.d, e2);
    float divisor = Dot(s1, e1);
    if (divisor == 0.f)
        return false;
    float invDivisor = 1.f / divisor;
    // Compute first barycentric coordinate
    Vector d = ray.o - p1;
    float b1 = Dot(d, s1) * invDivisor;
    if (b1 < 0.f || b1 > 1.f)
        return false;
    // Compute second barycentric coordinate
    Vector s2 = Cross(d, e1);
    float b2 = Dot(ray.d, s2) * invDivisor;
    if (b2 < 0.f || b1 + b2 > 1.f)
        return false;
    // Compute _t_ to intersection point
    float t = Dot(e2, s2) * invDivisor;
    if (t < ray.mint || t > ray.maxt)
        return false;

    return true;
}

float MeshBaryTriangle::Area() const {
    // Get triangle vertices in _p1_, _p2_, and _p3_
    const Point &p1 = mesh->p[v[0]];
    const Point &p2 = mesh->p[v[1]];
    const Point &p3 = mesh->p[v[2]];
    return 0.5f * Cross(p2-p1, p3-p1).Length();
}

Point MeshBaryTriangle::Sample(float u1, float u2,
        Normal *Ns) const {
    float b1, b2;
    UniformSampleTriangle(u1, u2, &b1, &b2);
    // Get triangle vertices in _p1_, _p2_, and _p3_
    const Point &p1 = mesh->p[v[0]];
    const Point &p2 = mesh->p[v[1]];
    const Point &p3 = mesh->p[v[2]];
    Point p = b1 * p1 + b2 * p2 + (1.f - b1 - b2) * p3;
    Normal n = Normal(Cross(p2-p1, p3-p1));
    *Ns = Normalize(n);
    if (mesh->reverseOrientation ^ mesh->transformSwapsHandedness)
    	*Ns *= -1.f;
    return p;
}

void MeshBaryTriangle::GetShadingGeometry(const Transform &obj2world,
		const DifferentialGeometry &dg,
		DifferentialGeometry *dgShading) const
{
	if (!mesh->n) {
		*dgShading = dg;
		return;
	}

	// Use _n_ and _s_ to compute shading tangents for triangle, _ss_ and _ts_
	Normal ns = dg.nn;
	Vector ss = Normalize(dg.dpdu);
	Vector ts = Normalize(Cross(ss, ns));
	ss = Cross(ts, ns);

	Vector dndu, dndv;
	// Compute \dndu and \dndv for triangle shading geometry
	float uvs[3][2];
	GetUVs(uvs);

	// Compute deltas for triangle partial derivatives of normal
	float du1 = uvs[0][0] - uvs[2][0];
	float du2 = uvs[1][0] - uvs[2][0];
	float dv1 = uvs[0][1] - uvs[2][1];
	float dv2 = uvs[1][1] - uvs[2][1];
	Vector dn1 = Vector(mesh->n[v[0]] - mesh->n[v[2]]);
	Vector dn2 = Vector(mesh->n[v[1]] - mesh->n[v[2]]);
	float determinant = du1 * dv2 - dv1 * du2;

	if (determinant == 0)
		dndu = dndv = Vector(0, 0, 0);
	else {
		float invdet = 1.f / determinant;
		dndu = ( dv2 * dn1 - dv1 * dn2) * invdet;
		dndv = (-du2 * dn1 + du1 * dn2) * invdet;

		dndu = mesh->ObjectToWorld(dndu);
		dndv = mesh->ObjectToWorld(dndu);
	}

	*dgShading = DifferentialGeometry(
			dg.p,
			ns,
			ss, ts,
			dndu, dndv,
			dg.u, dg.v, this);
	dgShading->reverseOrientation = mesh->reverseOrientation;
	dgShading->transformSwapsHandedness = mesh->transformSwapsHandedness;

	dgShading->dudx = dg.dudx;  dgShading->dvdx = dg.dvdx;
	dgShading->dudy = dg.dudy;  dgShading->dvdy = dg.dvdy;
	dgShading->dpdx = dg.dpdx;  dgShading->dpdy = dg.dpdy;
}
