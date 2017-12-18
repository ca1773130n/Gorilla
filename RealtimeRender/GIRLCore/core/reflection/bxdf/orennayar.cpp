/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   orennayar.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - orennayar.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// orennayar.cpp*
#include "orennayar.h"
#include "spectrum.h"
#include "mc.h"

using namespace girl;

void OrenNayar::f(const TsPack *tspack, const Vector &wo, 
				  const Vector &wi, SWCSpectrum *const f) const {
	float sinthetai = SinTheta(wi);
	float sinthetao = SinTheta(wo);
	// Compute cosine term of Oren--Nayar model
	float maxcos = 0.f;
	if (sinthetai > 1e-4 && sinthetao > 1e-4) {
		float sinphii = SinPhi(wi), cosphii = CosPhi(wi);
		float sinphio = SinPhi(wo), cosphio = CosPhi(wo);
		float dcos = cosphii * cosphio + sinphii * sinphio;
		maxcos = max(0.f, dcos);
	}
	// Compute sine and tangent terms of Oren--Nayar model
	float sinalpha, tanbeta;
	if (fabsf(CosTheta(wi)) > fabsf(CosTheta(wo))) {
		sinalpha = sinthetao;
		tanbeta = sinthetai / fabsf(CosTheta(wi));
	}
	else {
		sinalpha = sinthetai;
		tanbeta = sinthetao / fabsf(CosTheta(wo));
	}
	// *f += R * INV_PI * (A + B * maxcos * sinalpha * tanbeta);
	f->AddWeighted(INV_PI * (A + B * maxcos * sinalpha * tanbeta), R);
}

