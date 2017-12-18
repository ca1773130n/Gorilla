/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   material.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - material.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// material.cpp*
#include "material.h"
#include "shape.h"
#include "texture.h"
#include "geometry.h"

using namespace girl;

// Material Method Definitions
Material::~Material() {
}

void Material::Bump(boost::shared_ptr<Texture<float> > d,
		const DifferentialGeometry &dgGeom,
		const DifferentialGeometry &dgs,
		DifferentialGeometry *dgBump) {
	// Compute offset positions and evaluate displacement texture
	DifferentialGeometry dgEval = dgs;

	// Shift _dgEval_ _du_ in the $u$ direction and calculate bump map value
	float du = .5f * (fabsf(dgs.dudx) + fabsf(dgs.dudy));
	if (du == 0.f) du = .01f;
	dgEval.p += du * dgs.dpdu;
	dgEval.u += du;
	dgEval.nn = Normalize(dgs.nn + du * dgs.dndu);
	float uDisplace = d->Evaluate(NULL, dgEval);

	// Shift _dgEval_ _dv_ in the $v$ direction and calculate bump map value
	float dv = .5f * (fabsf(dgs.dvdx) + fabsf(dgs.dvdy));
	if (dv == 0.f) dv = .01f;
	dgEval.p = dgs.p + dv * dgs.dpdv;
	dgEval.u = dgs.u;
	dgEval.v += dv;
	dgEval.nn = Normalize(dgs.nn + dv * dgs.dndv);
	float vDisplace = d->Evaluate(NULL, dgEval);

	// Calculate bump map value at intersection point
	float displace = d->Evaluate(NULL, dgs);

	// Compute bump-mapped differential geometry
	*dgBump = dgs;
	dgBump->dpdu += (uDisplace - displace) / du * Vector(dgs.nn);   // different to book, as displace*dgs.dndu creates artefacts
	dgBump->dpdv += (vDisplace - displace) / dv * Vector(dgs.nn);   // different to book, as displace*dgs.dndv creates artefacts
	dgBump->nn = Normal(Normalize(Cross(dgBump->dpdu, dgBump->dpdv)));
	// INFO: We don't compute dgBump->dndu and dgBump->dndv as we need this
	//       only here.

	// Orient shading normal to match geometric normal
	if (Dot(dgGeom.nn, dgBump->nn) < 0.f)
		dgBump->nn *= -1.f;
}
