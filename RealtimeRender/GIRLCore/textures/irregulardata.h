/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   irregulardata.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - irregulardata.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// irregulardata.cpp*
#include "girl.h"
#include "texture.h"
#include "irregular.h"
#include "paramset.h"

namespace girl
{

// IrregularDataTexture Declarations
template <class T>
class GIRLCOREDLL  IrregularDataFloatTexture : public Texture<T> {
public:
	// IrregularDataFloatTexture Public Methods
	IrregularDataFloatTexture(const T &v) { value = v; }
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &) const {
		return value;
	}
private:
	T value;
};

template <class T>
class GIRLCOREDLL  IrregularDataSpectrumTexture : public Texture<T> {
public:
	// IrregularDataSpectrumTexture Public Methods
	IrregularDataSpectrumTexture(const int &n, const float *wl, const float *data) {
		SPD = new IrregularSPD(wl, data, n);
	}
	T Evaluate(const TsPack *tspack, const DifferentialGeometry &) const {
		return SWCSpectrum(tspack, SPD);
	}
	void SetPower(float power, float area) {
		SPD->Scale(power / (area * M_PI * SPD->y()));
	}
private:
	IrregularSPD* SPD;
};

class IrregularDataTexture
{
public:
	static Texture<float> * CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	static Texture<SWCSpectrum> * CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);
};

}//namespace girl

