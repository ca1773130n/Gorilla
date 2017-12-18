/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   windy.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - windy.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// windy.cpp*
#include "girl.h"
#include "texture.h"
#include "paramset.h"

// TODO - radiance - add methods for Power and Illuminant propagation

namespace girl
{

// WindyTexture Declarations
template <class T> class GIRLCOREDLL  WindyTexture : public Texture<T> {
public:
	// WindyTexture Public Methods
	~WindyTexture() {
		delete mapping;
	}
	WindyTexture(TextureMapping3D *map) {
		mapping = map;
	}
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &dg) const {
		Vector dpdx, dpdy;
		Point P = mapping->Map(dg, &dpdx, &dpdy);
		float windStrength =
			FBm(.1f * P, .1f * dpdx, .1f * dpdy, .5f, 3);
		float waveHeight =
			FBm(P, dpdx, dpdy, .5f, 6);
		return fabsf(windStrength) * waveHeight;
	}
	
	static Texture<float> * CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	static Texture<SWCSpectrum> * CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);
private:
	// WindyTexture Private Data
	TextureMapping3D *mapping;
};

// WindyTexture Method Definitions
template <class T> inline Texture<float> * WindyTexture<T>::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	// Initialize 3D texture mapping _map_ from _tp_
	TextureMapping3D *map = new IdentityMapping3D(tex2world);
	// Apply texture specified transformation option for 3D mapping
	IdentityMapping3D *imap = (IdentityMapping3D*) map;
	imap->Apply3DTextureMappingOptions(tp);
	return new WindyTexture<float>(map);
}

template <class T> inline Texture<SWCSpectrum> * WindyTexture<T>::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	// Initialize 3D texture mapping _map_ from _tp_
	TextureMapping3D *map = new IdentityMapping3D(tex2world);
	// Apply texture specified transformation option for 3D mapping
	IdentityMapping3D *imap = (IdentityMapping3D*) map;
	imap->Apply3DTextureMappingOptions(tp);
	return new WindyTexture<SWCSpectrum>(map);
}

}//namespace girl
