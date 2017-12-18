/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   harlequin.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - harlequin.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "girl.h"
#include "texture.h"
#include "color.h"
#include "sampling.h"
#include "paramset.h"
#include "primitive.h"

// TODO - radiance - add methods for Power and Illuminant propagation

namespace girl
{

// Dade - must be power of 2
#define HARLEQUIN_TEXTURE_PALETTE_SIZE 0x1f

// Harlequin Declarations
class GIRLCOREDLL  HarlequinTexture : public Texture<SWCSpectrum> {
public:
	// Harlequin Public Methods
	HarlequinTexture() {
		float c[3];
		for (int i = 0; i < HARLEQUIN_TEXTURE_PALETTE_SIZE; i++) {
			c[0] = RadicalInverse(i * COLOR_SAMPLES + 1, 2);
			c[1] = RadicalInverse(i * COLOR_SAMPLES + 1, 3);
			c[2] = RadicalInverse(i * COLOR_SAMPLES + 1, 5);

			ColorLookupTable[i] = RGBColor(c);
		}
	}
	~HarlequinTexture() { }

	SWCSpectrum Evaluate(const TsPack *tspack, const DifferentialGeometry &dg) const {
		// Dade - I assume object are 8 bytes aligned
		u_long lookupIndex = (((u_long)dg.prim) &
				((HARLEQUIN_TEXTURE_PALETTE_SIZE-1) << 3)) >> 3;

		return SWCSpectrum(tspack, ColorLookupTable[lookupIndex]);
	}

	static Texture<float> *CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	static Texture<SWCSpectrum> *CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);

private:
	static RGBColor ColorLookupTable[];
};

RGBColor HarlequinTexture::ColorLookupTable[HARLEQUIN_TEXTURE_PALETTE_SIZE];

// Harlequin Method Definitions
Texture<float> *HarlequinTexture::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return NULL;
}

Texture<SWCSpectrum> *HarlequinTexture::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return new HarlequinTexture();
}

}//namespace girl
