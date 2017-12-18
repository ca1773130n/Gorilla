/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   frequency.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - frequency.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// frequency.cpp*
#include "girl.h"
#include "texture.h"
#include "frequencyspd.h"
#include "paramset.h"

namespace girl
{

// FrequencyTexture Declarations
template <class T>
class GIRLCOREDLL  FrequencyFloatTexture : public Texture<T> {
public:
	// FrequencyTexture Public Methods
	FrequencyFloatTexture(const T &v) { value = v; }
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &) const {
		return value;
	}
private:
	T value;
};

template <class T>
class GIRLCOREDLL  FrequencySpectrumTexture : public Texture<T> {
public:
	// FrequencyTexture Public Methods
	FrequencySpectrumTexture(const float &w, const float &p, const float &r) {
		FSPD = new FrequencySPD(w, p, r);
	}
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &) const {
		return SWCSpectrum(tspack, FSPD);
	}
	void SetPower(float power, float area) {
		FSPD->Scale(power / (area * M_PI * FSPD->y()));
	}
private:
	FrequencySPD* FSPD;
};

class FrequencyTexture
{
public:
	static Texture<float> * CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	static Texture<SWCSpectrum> * CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);
};

}//namespace girl

