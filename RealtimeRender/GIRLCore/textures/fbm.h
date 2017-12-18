/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   fbm.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - fbm.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// fbm.cpp*
#include "girl.h"
#include "texture.h"
#include "paramset.h"

// TODO - radiance - add methods for Power and Illuminant propagation

namespace girl
{

// FBmTexture Declarations
template <class T> class GIRLCOREDLL  FBmTexture : public Texture<T> {
public:
	// FBmTexture Public Methods
	~FBmTexture() {
		delete mapping;
	}
	FBmTexture(int oct, float roughness, TextureMapping3D *map) {
		omega = roughness;
		octaves = oct;
		mapping = map;
	}
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &dg) const {
		Vector dpdx, dpdy;
		Point P = mapping->Map(dg, &dpdx, &dpdy);
		return FBm(P, dpdx, dpdy, omega, octaves);
	}
	
	static Texture<float> * CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	static Texture<SWCSpectrum> * CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);
	
private:
	// FBmTexture Private Data
	int octaves;
	float omega;
	TextureMapping3D *mapping;
};

// FBmTexture Method Definitions
template <class T> Texture<float> * FBmTexture<T>::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	// Initialize 3D texture mapping _map_ from _tp_
	TextureMapping3D *map = new IdentityMapping3D(tex2world);
	// Apply texture specified transformation option for 3D mapping
	IdentityMapping3D *imap = (IdentityMapping3D*) map;
	imap->Apply3DTextureMappingOptions(tp);

	return new FBmTexture<float>(tp.FindInt("octaves", 8),
		tp.FindFloat("roughness", .5f), map);
}

template <class T> Texture<SWCSpectrum> * FBmTexture<T>::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	// Initialize 3D texture mapping _map_ from _tp_
	TextureMapping3D *map = new IdentityMapping3D(tex2world);
	// Apply texture specified transformation option for 3D mapping
	IdentityMapping3D *imap = (IdentityMapping3D*) map;
	imap->Apply3DTextureMappingOptions(tp);

	return new FBmTexture<SWCSpectrum>(tp.FindInt("octaves", 8),
		tp.FindFloat("roughness", .5f), map);
}

}//namespace girl
