/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   mix.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - mix.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// mix.cpp*
#include "girl.h"
#include "texture.h"
#include "paramset.h"

namespace girl
{

// MixTexture Declarations
template <class T>
class GIRLCOREDLL  MixTexture : public Texture<T> {
public:
	// MixTexture Public Methods
	MixTexture(boost::shared_ptr<Texture<T> > t1,
			   boost::shared_ptr<Texture<T> > t2,
			   boost::shared_ptr<Texture<float> > amt) {
		tex1 = t1;
		tex2 = t2;
		amount = amt;
	}
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &dg) const {
		T t1 = tex1->Evaluate(tspack, dg), t2 = tex2->Evaluate(tspack, dg);
		float amt = amount->Evaluate(tspack, dg);
		return (1.f - amt) * t1 + amt * t2;
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
	boost::shared_ptr<Texture<T> > tex1, tex2;
	boost::shared_ptr<Texture<float> > amount;
};

// MixTexture Method Definitions
template <class T> Texture<float> * MixTexture<T>::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return new MixTexture<float>(
		tp.GetFloatTexture("tex1", 0.f),
		tp.GetFloatTexture("tex2", 1.f),
		tp.GetFloatTexture("amount", 0.5f));
}

template <class T> Texture<SWCSpectrum> * MixTexture<T>::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	return new MixTexture<SWCSpectrum>(
		tp.GetSWCSpectrumTexture("tex1", 0.f),
		tp.GetSWCSpectrumTexture("tex2", 1.f),
		tp.GetFloatTexture("amount", 0.5f));
}

}//namespace girl

