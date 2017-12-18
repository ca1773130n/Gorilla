/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   scale.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - scale.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////


// scale.cpp*
#include "girl.h"
#include "texture.h"
#include "paramset.h"

namespace girl
{

// ScaleTexture Declarations
template <class T1, class T2>
class GIRLCOREDLL  ScaleTexture : public Texture<T2> {
public:
	// ScaleTexture Public Methods
	ScaleTexture(boost::shared_ptr<Texture<T1> > t1,
			boost::shared_ptr<Texture<T2> > t2) {
		tex1 = t1;
		tex2 = t2;
	}
	T2 Evaluate(const TsPack *tspack, const DifferentialGeometry &dg) const {
		return tex1->Evaluate(tspack, dg) * tex2->Evaluate(tspack, dg);
	}
	void SetPower(float power, float area) {
		// Update sub-textures
		tex1->SetPower(power, area);
		tex2->SetPower(power, area);
	}
	void SetIlluminant() {
		// Update sub-textures
		tex1->SetIlluminant();
		tex2->SetIlluminant();
	}
	static Texture<float> * CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	static Texture<SWCSpectrum> * CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);
private:
	boost::shared_ptr<Texture<T1> > tex1;
	boost::shared_ptr<Texture<T2> > tex2;
};

// ScaleTexture Method Definitions
template <class T, class U> inline Texture<float> * ScaleTexture<T,U>::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return new ScaleTexture<float, float>(tp.GetFloatTexture("tex1", 1.f),
		tp.GetFloatTexture("tex2", 1.f));
}

template <class T,class U> inline Texture<SWCSpectrum> * ScaleTexture<T,U>::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return new ScaleTexture<SWCSpectrum, SWCSpectrum>(
		tp.GetSWCSpectrumTexture("tex1", RGBColor(1.f)),
		tp.GetSWCSpectrumTexture("tex2", RGBColor(1.f)));
}

}//namespace girl

