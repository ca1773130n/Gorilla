/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   blackbody.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - blackbody.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// blackbody.cpp*
#include "girl.h"
#include "texture.h"
#include "blackbodyspd.h"
#include "paramset.h"

namespace girl
{

// BlackBodyTexture Declarations
template <class T>
class GIRLCOREDLL  BlackBodyFloatTexture : public Texture<T> {
public:
	// BlackBodyTexture Public Methods
	BlackBodyFloatTexture(const T &v) { value = v; }
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &) const {
		return value;
	}
private:
	T value;
};

template <class T>
class GIRLCOREDLL  BlackBodySpectrumTexture : public Texture<T> {
public:
	// BlackBodyTexture Public Methods
	BlackBodySpectrumTexture(const float &t) {
		BBSPD = new BlackbodySPD(t);
	}
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &) const {
		return SWCSpectrum(tspack, BBSPD);
	}
	void SetPower(float power, float area) {
		BBSPD->Scale(power / (area * M_PI * BBSPD->y()));
	}

private:
	BlackbodySPD* BBSPD;
};

class BlackBodyTexture
{
public:
	static Texture<float> * CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	static Texture<SWCSpectrum> * CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);
};

}//namespace girl

