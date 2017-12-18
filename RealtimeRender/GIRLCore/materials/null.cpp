/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   null.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - null.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// null.cpp*
#include "null.h"
#include "bxdf.h"
#include "nulltransmission.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

// Glass Method Definitions
BSDF *Null::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const {
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs = dgShading;
	BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn, 1.);
	bsdf->Add(BSDF_ALLOC(tspack, NullTransmission)());
	return bsdf;
}
Material* Null::CreateMaterial(const Transform &xform,
		const TextureParams &mp) {
	return new Null();
}

static DynamicLoader::RegisterMaterial<Null> r("null");
