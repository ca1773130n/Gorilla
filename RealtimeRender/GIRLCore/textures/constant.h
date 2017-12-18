/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   constant.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - constant.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// constant.cpp*
#include "girl.h"
#include "texture.h"
#include "rgbrefl.h"
#include "rgbillum.h"
#include "paramset.h"

namespace girl
{

// ConstantTexture Declarations
template <class T>
class GIRLCOREDLL  ConstantFloatTexture : public Texture<T> {
public:
	// ConstantTexture Public Methods
	ConstantFloatTexture(const T &v) { textureType = TEXTURE_CONSTANT; value = v; }
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &) const {
		return value;
	}
	RGBColor GetRGB() { return RGBColor(0.f); }
private:
};

template <class T>
class GIRLCOREDLL  ConstantRGBColorTexture : public Texture<T> {
public:
	// ConstantTexture Public Methods
	ConstantRGBColorTexture(const RGBColor &s) {
		textureType = TEXTURE_CONSTANT;
		color = s;
		RGBSPD = new RGBReflSPD(color);
		rgb = s;
	}
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &) const {
		return SWCSpectrum(tspack, RGBSPD);
	}
	void SetPower(float power, float area) {
		RGBSPD->Scale(power / (area * M_PI * RGBSPD->y()));
	}
	void SetIlluminant() {
		delete RGBSPD;
		RGBSPD = new RGBIllumSPD(color);
	}
	RGBColor GetRGB() { return rgb; }
private:
	SPD* RGBSPD;
	RGBColor color;
};

class Constant
{
public:
	static Texture<float> * CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	static Texture<SWCSpectrum> * CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);
};

}//namespace girl

