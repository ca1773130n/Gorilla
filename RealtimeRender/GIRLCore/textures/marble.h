/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   marble.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - marble.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// marble.cpp*
#include "girl.h"
#include "texture.h"
#include "paramset.h"

// TODO - radiance - add methods for Power and Illuminant propagation

namespace girl
{

// MarbleTexture Declarations
class GIRLCOREDLL  MarbleTexture : public Texture<SWCSpectrum> {
public:
	// MarbleTexture Public Methods
	~MarbleTexture() {
		delete mapping;
	}
	MarbleTexture(int oct, float roughness, float sc, float var,
			TextureMapping3D *map) {
		omega = roughness;
		octaves = oct;
		mapping = map;
		scale = sc;
		variation = var;
	}
	SWCSpectrum Evaluate(const TsPack *tspack, const DifferentialGeometry &dg) const {
		Vector dpdx, dpdy;
		Point P = mapping->Map(dg, &dpdx, &dpdy);
		P *= scale;
		float marble = P.y + variation * FBm(P, scale * dpdx,
			scale * dpdy, omega, octaves);
		float t = .5f + .5f * sinf(marble);
		// Evaluate marble spline at _t_
		static float c[][3] = { { .58f, .58f, .6f }, { .58f, .58f, .6f }, { .58f, .58f, .6f },
			{ .5f, .5f, .5f }, { .6f, .59f, .58f }, { .58f, .58f, .6f },
			{ .58f, .58f, .6f }, {.2f, .2f, .33f }, { .58f, .58f, .6f }, };
		#define NC  sizeof(c) / sizeof(c[0])
		#define NSEG (NC-3)
		int first = Floor2Int(t * NSEG);
		t = (t * NSEG - first);
		RGBColor c0(c[first]), c1(c[first+1]), c2(c[first+2]), c3(c[first+3]);
		// Bezier spline evaluated with de Castilejau's algorithm
		RGBColor s0 = (1.f - t) * c0 + t * c1;
		RGBColor s1 = (1.f - t) * c1 + t * c2;
		RGBColor s2 = (1.f - t) * c2 + t * c3;
		s0 = (1.f - t) * s0 + t * s1;
		s1 = (1.f - t) * s1 + t * s2;
		// Extra scale of 1.5 to increase variation among colors
		return SWCSpectrum(tspack, 1.5f * ((1.f - t) * s0 + t * s1));
	}
	
	static Texture<float> * CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	static Texture<SWCSpectrum> * CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);
private:
	// MarbleTexture Private Data
	int octaves;
	float omega, scale, variation;
	TextureMapping3D *mapping;
};

}//namespace girl

