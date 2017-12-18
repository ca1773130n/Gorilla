/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   regulardata.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - regulardata.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// regulardata.cpp*
#include "girl.h"
#include "texture.h"
#include "regular.h"
#include "paramset.h"

namespace girl
{

// RegularDataTexture Declarations
template <class T>
class GIRLCOREDLL  RegularDataFloatTexture : public Texture<T> {
public:
	// RegularDataFloatTexture Public Methods
	RegularDataFloatTexture(const T &v) { value = v; }
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &) const {
		return value;
	}
private:
	T value;
};

template <class T>
class GIRLCOREDLL  RegularDataSpectrumTexture : public Texture<T> {
public:
	// RegularDataSpectrumTexture Public Methods
	RegularDataSpectrumTexture(const float &s, const float &e, const int &n, const float *data) {
		SPD = new RegularSPD(data, s, e, n);
	}
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &) const {
		return SWCSpectrum(tspack, SPD);
	}
	void SetPower(float power, float area) {
		SPD->Scale(power / (area * M_PI * SPD->y()));
	}
private:
	RegularSPD* SPD;
};

class RegularDataTexture
{
public:
	static Texture<float> * CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	static Texture<SWCSpectrum> * CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);
};

}//namespace girl

