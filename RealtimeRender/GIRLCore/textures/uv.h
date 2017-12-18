/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   uv.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - uv.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// uv.cpp*
#include "girl.h"
#include "texture.h"
#include "paramset.h"

// TODO - radiance - add methods for Power and Illuminant propagation

namespace girl
{

// UVTexture Declarations
class GIRLCOREDLL  UVTexture : public Texture<SWCSpectrum> {
public:
	// UVTexture Public Methods
	UVTexture(TextureMapping2D *m) {
		mapping = m;
	}
	~UVTexture() {
		delete mapping;
	}
	SWCSpectrum Evaluate(const TsPack *tspack, const DifferentialGeometry &dg) const {
		float s, t, dsdx, dtdx, dsdy, dtdy;
		mapping->Map(dg, &s, &t, &dsdx, &dtdx, &dsdy, &dtdy);
		float cs[COLOR_SAMPLES];
		memset(cs, 0, COLOR_SAMPLES * sizeof(float));
		cs[0] = s - Floor2Int(s);
		cs[1] = t - Floor2Int(t);
		return SWCSpectrum(tspack, RGBColor(cs));
	}
	
	static Texture<float> * CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	static Texture<SWCSpectrum> * CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);
private:
	TextureMapping2D *mapping;
};

}//namespace girl

