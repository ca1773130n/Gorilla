/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   gaussian.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - gaussian.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// gaussian.cpp*
#include "girl.h"
#include "texture.h"
#include "gaussianspd.h"
#include "paramset.h"

namespace girl
{

// GaussianTexture Declarations
template <class T>
class GIRLCOREDLL  GaussianFloatTexture : public Texture<T> {
public:
	// GaussianTexture Public Methods
	GaussianFloatTexture(const T &v) { value = v; }
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &) const {
		return value;
	}
private:
	T value;
};

template <class T>
class GIRLCOREDLL  GaussianSpectrumTexture : public Texture<T> {
public:
	// GaussianTexture Public Methods
	GaussianSpectrumTexture(const float &m, const float &w, const float &r) {
		GSPD = new GaussianSPD(m, w, r);
	}
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &) const {
		return SWCSpectrum(tspack, GSPD);
	}
	void SetPower(float power, float area) {
		GSPD->Scale(power / (area * M_PI * GSPD->y()));
	}
private:
	GaussianSPD* GSPD;
};

class GaussianTexture
{
public:
	static Texture<float> * CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	static Texture<SWCSpectrum> * CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);
};

}//namespace girl

