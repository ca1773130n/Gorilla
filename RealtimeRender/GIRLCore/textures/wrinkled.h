/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   wrinkled.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - wrinkled.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// wrinkled.cpp*
#include "girl.h"
#include "texture.h"
#include "paramset.h"

// TODO - radiance - add methods for Power and Illuminant propagation

namespace girl
{

// WrinkledTexture Declarations
template <class T> class GIRLCOREDLL  WrinkledTexture : public Texture<T> {
public:
	// WrinkledTexture Public Methods
	~WrinkledTexture() {
		delete mapping;
	}
	WrinkledTexture(int oct, float roughness, TextureMapping3D *map) {
		omega = roughness;
		octaves = oct;
		mapping = map;
	}
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &dg) const {
		Vector dpdx, dpdy;
		Point P = mapping->Map(dg, &dpdx, &dpdy);
		return Turbulence(P, dpdx, dpdy, omega, octaves);
	}
	
	static Texture<float> * CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	static Texture<SWCSpectrum> * CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);
private:
	// WrinkledTexture Private Data
	int octaves;
	float omega;
	TextureMapping3D *mapping;
};


// WrinkledTexture Method Definitions
template <class T> inline Texture<float> * WrinkledTexture<T>::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	// Initialize 3D texture mapping _map_ from _tp_
	TextureMapping3D *map = new IdentityMapping3D(tex2world);
	// Apply texture specified transformation option for 3D mapping
	IdentityMapping3D *imap = (IdentityMapping3D*) map;
	imap->Apply3DTextureMappingOptions(tp);
	return new WrinkledTexture<float>(tp.FindInt("octaves", 8),
		tp.FindFloat("roughness", .5f), map);
}

template <class T> inline Texture<SWCSpectrum> * WrinkledTexture<T>::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	// Initialize 3D texture mapping _map_ from _tp_
	TextureMapping3D *map = new IdentityMapping3D(tex2world);
	// Apply texture specified transformation option for 3D mapping
	IdentityMapping3D *imap = (IdentityMapping3D*) map;
	imap->Apply3DTextureMappingOptions(tp);
	return new WrinkledTexture<SWCSpectrum>(tp.FindInt("octaves", 8),
		tp.FindFloat("roughness", .5f), map);
}

}//namespace girl

