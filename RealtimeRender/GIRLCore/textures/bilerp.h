/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   bilerp.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - bilerp.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// bilerp.cpp*
#include "girl.h"
#include "texture.h"
#include "paramset.h"
#include "error.h"

namespace girl
{

// BilerpTexture Declarations
class GIRLCOREDLL  BilerpFloatTexture : public Texture<float> {
public:
	// BilerpTexture Public Methods
	BilerpFloatTexture(TextureMapping2D *m,
				  const float &t00, const float &t01,
			      const float &t10, const float &t11) {
		mapping = m;
		v00 = t00;
		v01 = t01;
		v10 = t10;
		v11 = t11;
	}
	~BilerpFloatTexture() {
		delete mapping;
	}
	float Evaluate(const TsPack *tspack, const DifferentialGeometry &dg) const {
		float s, t, dsdx, dtdx, dsdy, dtdy;
		mapping->Map(dg, &s, &t, &dsdx, &dtdx, &dsdy, &dtdy);
		return (1-s)*(1-t) * v00 +
		       (1-s)*(  t) * v01 +
			   (  s)*(1-t) * v10 +
			   (  s)*(  t) * v11;
	}
	
	static Texture<float> * CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	
private:
	// BilerpTexture Private Data
	TextureMapping2D *mapping;
	float v00, v01, v10, v11;
};

class GIRLCOREDLL  BilerpSpectrumTexture : public Texture<SWCSpectrum> {
public:
	// BilerpTexture Public Methods
	BilerpSpectrumTexture(TextureMapping2D *m,
				  const RGBColor &t00, const RGBColor &t01,
			      const RGBColor &t10, const RGBColor &t11) {
		mapping = m;
		v00 = t00;
		v01 = t01;
		v10 = t10;
		v11 = t11;
	}
	~BilerpSpectrumTexture() {
		delete mapping;
	}
	SWCSpectrum Evaluate(const TsPack *tspack, const DifferentialGeometry &dg) const {
		float s, t, dsdx, dtdx, dsdy, dtdy;
		mapping->Map(dg, &s, &t, &dsdx, &dtdx, &dsdy, &dtdy);
		return SWCSpectrum(tspack, (1-s)*(1-t) * v00 +
		       (1-s)*(  t) * v01 +
			   (  s)*(1-t) * v10 +
			   (  s)*(  t) * v11 );
	}
	
	static Texture<SWCSpectrum> * CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);
	
private:
	// BilerpTexture Private Data
	TextureMapping2D *mapping;
	RGBColor v00, v01, v10, v11;
};

}//namespace girl

