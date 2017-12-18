/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   equalenergy.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - equalenergy.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// equalenergy.cpp*
#include "girl.h"
#include "texture.h"
#include "equalspd.h"
#include "paramset.h"

namespace girl
{

// EqualEnergyTexture Declarations
template <class T>
class GIRLCOREDLL  EqualEnergyFloatTexture : public Texture<T> {
public:
	// EqualEnergyTexture Public Methods
	EqualEnergyFloatTexture(const T &v) { value = v; }
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &) const {
		return value;
	}
private:
	T value;
};

template <class T>
class GIRLCOREDLL  EqualEnergySpectrumTexture : public Texture<T> {
public:
	// EqualEnergyTexture Public Methods
	EqualEnergySpectrumTexture(const float &t) {
		e = t;
		weight = 1.f;
	}
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &) const {
		return SWCSpectrum(e * weight);
	}
	void SetPower(float power, float area) {
		weight = power / (area * M_PI * 1.f);
	}
private:
	float e;
	float weight;
};

class EqualEnergyTexture
{
public:
	static Texture<float> * CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	static Texture<SWCSpectrum> * CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);
};

}//namespace girl

