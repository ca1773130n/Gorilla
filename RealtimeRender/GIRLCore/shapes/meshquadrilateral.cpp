/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   meshquadrilateral.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - meshquadrilateral.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "mesh.h"

using namespace girl;

// checks if quad is degenerate or if any points coincide
bool MeshQuadrilateral::IsDegenerate(const Point &p0, const Point &p1, const Point &p2, const Point &p3) {

	Vector e0 = p1 - p0;
	Vector e1 = p2 - p1;
	Vector e2 = p3 - p2;
	Vector e3 = p0 - p3;

	float el0 = e0.Length();
	float el1 = e1.Length();
	float el2 = e2.Length();
	float el3 = e3.Length();

	return el0 < 1e-30 || el1 < 1e-30 || el2 < 1e-30 || el3 < 1e-30;
}

// checks if a non-degenerate quad is planar
// most likely susceptible to numerical issues for large quads
bool MeshQuadrilateral::IsPlanar(const Point &p0, const Point &p1, const Point &p2, const Point &p3) {

	// basis vectors for projection
	Vector e0 = p1 - p0;
	Vector e1 = p2 - p0;

	Point p = p3;

	if (1.f - fabsf(Dot(e0, e1)) < 1e-6) {
		// if collinear, use p3
		e1 = p3 - p0;
		p = p2;
	}

	Vector n = Cross(e1, e0);

	Vector x = p - p0;

	// find distance from point to plane defined by e0 and e1
	float D = fabsf(Dot(x, n));

	// if planar, the distance from point to plane should be zero
	// |x.n|/|n| < eps ==> |x.n| < |n| * eps
	return D < n.Length() * 1e-6f;
}

// checks if a non-degenerate, planar quad is strictly convex
bool MeshQuadrilateral::IsConvex(const Point &p0, const Point &p1, const Point &p2, const Point &p3) {

	// basis vectors for plane
	Vector b0 = Normalize(p1 - p0);
	Vector b1 = p3 - p0;
	// orthogonalize using Gram-Schmitdt
	b1 = Normalize(b1 - b0 * Dot(b1, b0));

	if (1.f - fabsf(Dot(b0, b1)) < 1e-6) {
		// if collinear, use p2
		b1 = p2 - p0;
		// orthogonalize using Gram-Schmitdt
		b1 = Normalize(b1 - b0 * Dot(b1, b0));
	}

	// compute polygon edges
	Vector e[4];

	e[0] = p1 - p0;
	e[1] = p2 - p1;
	e[2] = p3 - p2;
	e[3] = p0 - p3;

	// project edges onto the plane
	for (int i = 0; i < 4; i++)
		e[i] = Vector(Dot(e[i], b0), Dot(e[i], b1), 0);

	// in a convex polygon, the x values should alternate between
	// increasing and decreasing exactly twice
	int altCount = 0;
	int curd, prevd;

	// since b0 is constructed from the same edge as e0
	// it's x component will always be positive (|e0| is always > 0)
	// this is just a boot-strap, hence i=1..4 below
	curd = 1;
	for (int i = 1; i <= 4; i++) {
		prevd = curd;
		// if x component of edge is zero, we simply ignore it by
		// using the previous direction
		curd = (e[i & 3].x < 1e-6) ? (e[i & 3].x > -1e-6f ? prevd : -1) : 1;
		altCount += prevd != curd ? 1 : 0;
	}

	if (altCount != 2)
		return false;

	// some concave polygons might pass
	// the above test, verify that the turns
	// all go in the same direction
	int curs, prevs;
	altCount = 0;

	curs = (Cross(e[1], e[0]).z < 0) ? -1 : 1;
	for (int i = 1; i < 4; i++) {
		prevs = curs;
		curs = (Cross(e[(i + 1) & 3], e[i]).z < 0) ? -1 : 1;
		altCount += prevs != curs ? 1 : 0;
	}

	return altCount == 0;
}

float MeshQuadrilateral::Det2x2(const float a00, const float a01, const float a10, const float a11) {
	return a00*a11 - a01*a10;
}

float MeshQuadrilateral::Det3x3(float A[3][3]) {
	return
		A[0][0] * Det2x2(A[1][1], A[1][2], A[2][1], A[2][2]) -
		A[0][1] * Det2x2(A[1][0], A[1][2], A[2][0], A[2][2]) +
		A[0][2] * Det2x2(A[1][0], A[1][1], A[2][0], A[2][1]);
}

bool MeshQuadrilateral::Invert3x3(float A[3][3], float InvA[3][3]) {

	float determinant = Det3x3(A);
	if (determinant == 0.f)
		return false;

	float invdet = 1.f / determinant;

	InvA[0][0] = invdet * Det2x2(A[1][1], A[1][2], A[2][1], A[2][2]);
	InvA[0][1] = invdet * Det2x2(A[0][2], A[0][1], A[2][2], A[2][1]);
	InvA[0][2] = invdet * Det2x2(A[0][1], A[0][2], A[1][1], A[1][2]);
	InvA[1][0] = invdet * Det2x2(A[1][2], A[1][0], A[2][2], A[2][0]);
	InvA[1][1] = invdet * Det2x2(A[0][0], A[0][2], A[2][0], A[2][2]);
	InvA[1][2] = invdet * Det2x2(A[0][2], A[0][0], A[1][2], A[1][0]);
	InvA[2][0] = invdet * Det2x2(A[1][0], A[1][1], A[2][0], A[2][1]);
	InvA[2][1] = invdet * Det2x2(A[0][1], A[0][0], A[2][1], A[2][0]);
	InvA[2][2] = invdet * Det2x2(A[0][0], A[0][1], A[1][0], A[1][1]);

	return true;
}

int MeshQuadrilateral::MajorAxis(const Vector &v) {
	float absVx = fabsf(v.x);
	float absVy = fabsf(v.y);
	float absVz = fabsf(v.z);

	if (absVx > absVy)
		return (absVx > absVz) ? 0 : 2;
	return (absVy > absVz) ? 1 : 2;
}

void MeshQuadrilateral::ComputeV11BarycentricCoords(const Vector &e01,
	const Vector &e02, const Vector &e03, float *a11, float *b11) {
		const Vector N = Cross(e01, e03);

	int Nma = MajorAxis(N);

	switch (Nma) {
		case 0: {
			float iNx = 1.f / N.x;
			*a11 = (e02.y * e03.z - e02.z * e03.y) * iNx;
			*b11 = (e01.y * e02.z - e01.z * e02.y) * iNx;
			break;
		}
		case 1: {
			float iNy = 1.f / N.y;
			*a11 = (e02.z * e03.x - e02.x * e03.z) * iNy;
			*b11 = (e01.z * e02.x - e01.x * e02.z) * iNy;
			break;
		}
		case 2: {
			float iNz = 1.f / N.z;
			*a11 = (e02.x * e03.y - e02.y * e03.x) * iNz;
			*b11 = (e01.x * e02.y - e01.y * e02.x) * iNz;
			break;
		}
		default:
			BOOST_ASSERT(false);
			// since we don't allow for degenerate quads the normal
			// should always be well defined and we should never get here
			break;
	}
}

//------------------------------------------------------------------------------
MeshQuadrilateral::MeshQuadrilateral(const Mesh *m, int n)
	: mesh(m), idx(&(mesh->quadVertexIndex[4 * n]))
{
	// LordCrc - check for problematic quads
	const Point &p0 = mesh->WorldToObject(mesh->p[idx[0]]);
    const Point &p1 = mesh->WorldToObject(mesh->p[idx[1]]);
    const Point &p2 = mesh->WorldToObject(mesh->p[idx[2]]);
    const Point &p3 = mesh->WorldToObject(mesh->p[idx[3]]);

	if (IsDegenerate(p0, p1, p2, p3)) {
		girlError(GIRL_CONSISTENCY, GIRL_ERROR, string("Degenerate quadrilateral detected").c_str());
		idx = NULL;
	}
	else if (!IsPlanar(p0, p1, p2, p3)) {
		girlError(GIRL_CONSISTENCY, GIRL_ERROR, string("Non-planar quadrilateral detected").c_str());
		idx = NULL;
	}
	else if (!IsConvex(p0, p1, p2, p3)) {
		girlError(GIRL_CONSISTENCY, GIRL_ERROR, string("Non-convex quadrilateral detected").c_str());
		idx = NULL;
	}

	if (!idx)
		return;

	// Dade - reorder the vertices if required
	for(int i = 0; i < 4; i++) {
		// Get quadrilateral vertices in _p00_, _p10_, _p11_ and _p01_
		const Point &p00 = mesh->p[idx[0]];
		const Point &p10 = mesh->p[idx[1]];
		const Point &p11 = mesh->p[idx[2]];
		const Point &p01 = mesh->p[idx[3]];

		// Compute the barycentric coordinates of V11.
		const Vector e01 = p10 - p00;
		const Vector e03 = p01 - p00;
		const Vector e02 = p11 - p00;
		const Vector N = Cross(e01, e03);

		float a11 = 0.0f;
		float b11 = 0.0f;

		ComputeV11BarycentricCoords(e01, e02, e03, &a11, &b11);

		if ((a11 > 1.0f) || (b11 > 1.0f)) {
			// Dade - we need to reorder the vertices

			// Dade - this code has as side effect to reorder the indices
			// in mesh->quadVertexIndex. It is not a very clean behavior but
			// it is simple and fast

			int* nonConstIdx = const_cast<int*>(idx);
			const int tmp = nonConstIdx[0];
			nonConstIdx[0] = nonConstIdx[1];
			nonConstIdx[1] = nonConstIdx[2];
			nonConstIdx[2] = nonConstIdx[3];
			nonConstIdx[3] = tmp;
		} else
			break;
	}
}

BBox MeshQuadrilateral::ObjectBound() const {

	if (!idx)
		return BBox();

	// Get quadrilateral vertices in _p0_, _p1_, _p2_, and _p3_
    const Point &p0 = mesh->p[idx[0]];
    const Point &p1 = mesh->p[idx[1]];
    const Point &p2 = mesh->p[idx[2]];
    const Point &p3 = mesh->p[idx[3]];

	return Union(BBox(mesh->WorldToObject(p0), mesh->WorldToObject(p1)),
            BBox(mesh->WorldToObject(p2), mesh->WorldToObject(p3)));
}

BBox MeshQuadrilateral::WorldBound() const {

	if (!idx)
		return BBox();

	// Get quadrilateral vertices in _p0_, _p1_, _p2_, and _p3_
    const Point &p0 = mesh->p[idx[0]];
    const Point &p1 = mesh->p[idx[1]];
    const Point &p2 = mesh->p[idx[2]];
    const Point &p3 = mesh->p[idx[3]];

    return Union(BBox(p0, p1), BBox(p2, p3));
}

bool MeshQuadrilateral::Intersect(const Ray &ray, Intersection *isect) const {
	// Compute intersection for quadrilateral
	// based on "An Efficient Ray-Quadrilateral Intersection Test"
	// by Ares Lagae and Philip Dutr�?
	// http://www.cs.kuleuven.be/~graphics/CGRG.PUBLICATIONS/LagaeDutre2005AnEfficientRayQuadrilateralIntersectionTest/
	// http://jgt.akpeters.com/papers/LagaeDutre05/erqit.cpp.html

	if (!idx)
		return false;

    // Get quadrilateral vertices in _p00_, _p10_, _p11_ and _p01_
    const Point &p00 = mesh->p[idx[0]];
    const Point &p10 = mesh->p[idx[1]];
    const Point &p11 = mesh->p[idx[2]];
    const Point &p01 = mesh->p[idx[3]];

	// Reject rays using the barycentric coordinates of
	// the intersection point with respect to T.
	const Vector e01 = p10 - p00;
	const Vector e03 = p01 - p00;
	const Vector P = Cross(ray.d, e03);
	float det = Dot(e01, P);
	if (fabsf(det) < 1e-7)
		return false;

	float invdet = 1.f / det;

	const Vector T = ray.o - p00;
	float alpha = Dot(T, P) * invdet;
	if (alpha < 0)// || alpha > 1)
		return false;

	const Vector Q = Cross(T, e01);
	float beta = Dot(ray.d, Q) * invdet;
	if (beta < 0)// || beta > 1)
		return false;

	// Reject rays using the barycentric coordinates of
	// the intersection point with respect to T'.
	if ((alpha + beta) > 1.f) {
		const Vector e23 = p01 - p11;
		const Vector e21 = p10 - p11;
		const Vector P2 = Cross(ray.d, e21);
		float det2 = Dot(e23, P2);
		if (fabsf(det2) < 1e-7f)
			return false;
		//float invdet2 = 1.f / det2;
		// since we only reject if alpha or beta < 0
		// we just need the sign info from det2
		float invdet2 = (det2 < 0) ? -1 : 1;
		const Vector T2 = ray.o - p11;
		float alpha2 = Dot(T2, P2) * invdet2;
		if (alpha2 < 0)
			return false;
		const Vector Q2 = Cross(T2, e23);
		float beta2 = Dot(ray.d, Q2) * invdet2;
		if (beta2 < 0)
			return false;
	}

	// Compute the ray parameter of the intersection
	// point.
	float t = Dot(e03, Q) * invdet;
	if (t < ray.mint || t > ray.maxt)
		return false;

	// Compute the barycentric coordinates of V11.
	const Vector e02 = p11 - p00;

	float a11 = 0.f;
	float b11 = 0.f;

	ComputeV11BarycentricCoords(e01, e02, e03, &a11, &b11);

	// save a lot of redundant computations
	a11 = a11 - 1;
	b11 = b11 - 1;

	// Compute the bilinear coordinates of the
	// intersection point.
	float u = 0.0f, v = 0.0f;
	if (fabsf(a11) < 1e-7f) {
		u = alpha;
		v = fabsf(b11) < 1e-7f ? beta : beta / (u * b11 + 1.f);
	} else if (fabsf(b11) < 1e-7f) {
		v = beta;
		u = alpha / (v * a11 + 1.f);
	} else {
		float A = -b11;
		float B = alpha*b11 - beta*a11 - 1.f;
		float C = alpha;

		Quadratic(A, B, C, &u, &v);
		if ((u < 0) || (u > 1))
			u = v;

		v = beta / (u * b11 + 1.f);
	}

	// compute partial differentials
	// see bugtracker ID 324 for derivation
	Vector dpdu, dpdv;
	float uv[4][2];
	float A[3][3], InvA[3][3];

	GetUVs(uv);

	A[0][0] = uv[1][0] - uv[0][0];
	A[0][1] = uv[1][1] - uv[0][1];
	A[0][2] = uv[1][0]*uv[1][1] - uv[0][0]*uv[0][1];
	A[1][0] = uv[2][0] - uv[0][0];
	A[1][1] = uv[2][1] - uv[0][1];
	A[1][2] = uv[2][0]*uv[2][1] - uv[0][0]*uv[0][1];
	A[2][0] = uv[3][0] - uv[0][0];
	A[2][1] = uv[3][1] - uv[0][1];
	A[2][2] = uv[3][0]*uv[3][1] - uv[0][0]*uv[0][1];

	// invert matrix
	if (!Invert3x3(A, InvA)) {
        // Handle zero determinant for quadrilateral partial derivative matrix
		Vector N = Cross(e01, e02);
        CoordinateSystem(Normalize(N), &dpdu, &dpdv);
	} else {
		dpdu = Vector(
			InvA[0][0] * e01.x + InvA[0][1] * e02.x + InvA[0][2] * e03.x,
			InvA[0][0] * e01.y + InvA[0][1] * e02.y + InvA[0][2] * e03.y,
			InvA[0][0] * e01.z + InvA[0][1] * e02.z + InvA[0][2] * e03.z);
		dpdv = Vector(
			InvA[1][0] * e01.x + InvA[1][1] * e02.x + InvA[1][2] * e03.x,
			InvA[1][0] * e01.y + InvA[1][1] * e02.y + InvA[1][2] * e03.y,
			InvA[1][0] * e01.z + InvA[1][1] * e02.z + InvA[1][2] * e03.z);
	}

	// Dade - using the intepolated normal here in order to fix bug #340
	Normal nn;
	if (mesh->n)
		nn = Normalize(mesh->ObjectToWorld(
				((1.0f - u) * (1.0f - v)) * mesh->n[idx[0]] +
				(u * (1.0f - v)) * mesh->n[idx[1]] +
				(u * v) * mesh->n[idx[2]] +
				((1.0f - u) * v) * mesh->n[idx[3]]));
	else {
		Vector N = Cross(e01, e02);
		nn = Normal(Normalize(N));
	}

	if(isect) {
		isect->dg = DifferentialGeometry(ray(t),
				nn,
				dpdu, dpdv,
				Vector(0, 0, 0), Vector(0, 0, 0),
				u, v, this);
		isect->dg.AdjustNormal(mesh->reverseOrientation, mesh->transformSwapsHandedness);
		isect->Set(mesh->ObjectToWorld, this, mesh->GetMaterial().get());
		ray.maxt = t;
	}

	return true;
}

bool MeshQuadrilateral::IntersectP(const Ray &ray) const {
	return Intersect(ray, NULL);
}

float MeshQuadrilateral::Area() const {

	if (!idx)
		return 0.f;

	// assumes convex quadrilateral
    const Point &p0 = mesh->p[idx[0]];
    const Point &p1 = mesh->p[idx[1]];
    const Point &p3 = mesh->p[idx[3]];

	Vector P = p1 - p0;
	Vector Q = p3 - p1;

	Vector PxQ = Cross(P, Q);

	return 0.5f * PxQ.Length();
}

void MeshQuadrilateral::GetShadingGeometry(const Transform &obj2world,
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

	// compute partial differentials
	// see bugtracker ID 324 for derivation
	Vector dndu, dndv;
	float uv[4][2];
	float A[3][3], InvA[3][3];

	GetUVs(uv);

	A[0][0] = uv[1][0] - uv[0][0];
	A[0][1] = uv[1][1] - uv[0][1];
	A[0][2] = uv[1][0]*uv[1][1] - uv[0][0]*uv[0][1];
	A[1][0] = uv[2][0] - uv[0][0];
	A[1][1] = uv[2][1] - uv[0][1];
	A[1][2] = uv[2][0]*uv[2][1] - uv[0][0]*uv[0][1];
	A[2][0] = uv[3][0] - uv[0][0];
	A[2][1] = uv[3][1] - uv[0][1];
	A[2][2] = uv[3][0]*uv[3][1] - uv[0][0]*uv[0][1];

	// invert matrix
	if (!Invert3x3(A, InvA)) {
        // Handle zero determinant for quadrilateral partial derivative matrix
		dndu = dndv = Vector(0, 0, 0);
	} else {
		const Normal &n00 = mesh->n[idx[0]];
		const Normal &n10 = mesh->n[idx[1]];
		const Normal &n11 = mesh->n[idx[2]];
		const Normal &n01 = mesh->n[idx[3]];

		const Vector dn01 = Vector(n10 - n00);
		const Vector dn02 = Vector(n11 - n00);
		const Vector dn03 = Vector(n01 - n00);

		dndu = Vector(
			InvA[0][0] * dn01.x + InvA[0][1] * dn02.x + InvA[0][2] * dn03.x,
			InvA[0][0] * dn01.y + InvA[0][1] * dn02.y + InvA[0][2] * dn03.y,
			InvA[0][0] * dn01.z + InvA[0][1] * dn02.z + InvA[0][2] * dn03.z);
		dndv = Vector(
			InvA[1][0] * dn01.x + InvA[1][1] * dn02.x + InvA[1][2] * dn03.x,
			InvA[1][0] * dn01.y + InvA[1][1] * dn02.y + InvA[1][2] * dn03.y,
			InvA[1][0] * dn01.z + InvA[1][1] * dn02.z + InvA[1][2] * dn03.z);
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
