/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   spectrum.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - spectrum.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_RGBColor_H
#define GIRL_RGBColor_H
// spectrum.h*
#include "girl.h"
#include "color.h"

namespace girl
{

#define WAVELENGTH_SAMPLES 4
#define WAVELENGTH_START 380.
#define WAVELENGTH_END   720.
static const float inv_WAVELENGTH_SAMPLES = 1.f / WAVELENGTH_SAMPLES;

#define Scalar float

class GIRLCOREDLL  SWCSpectrum {
	friend class boost::serialization::access;
public:
	// RGBColor Public Methods
	SWCSpectrum(Scalar v = 0.f) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] = v;
	}
	SWCSpectrum(const TsPack *tspack, RGBColor s);							// Note -radiance- - REFACT - can inline now.

	SWCSpectrum(const TsPack *tspack, const SPD *s);

	SWCSpectrum(float cs[WAVELENGTH_SAMPLES]) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] = cs[i];
	}
	friend ostream &operator<<(ostream &, const SWCSpectrum &);
	SWCSpectrum &operator+=(const SWCSpectrum &s2) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] += s2.c[i];
		return *this;
	}
	SWCSpectrum operator+(const SWCSpectrum &s2) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] += s2.c[i];
		return ret;
	}
	SWCSpectrum operator-(const SWCSpectrum &s2) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] -= s2.c[i];
		return ret;
	}
	SWCSpectrum operator/(const SWCSpectrum &s2) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] /= s2.c[i];
		return ret;
	}
	SWCSpectrum operator*(const SWCSpectrum &sp) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] *= sp.c[i];
		return ret;
	}
	SWCSpectrum &operator*=(const SWCSpectrum &sp) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] *= sp.c[i];
		return *this;
	}
	SWCSpectrum operator*(Scalar a) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] *= a;
		return ret;
	}
	SWCSpectrum &operator*=(Scalar a) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] *= a;
		return *this;
	}
	friend inline
	SWCSpectrum operator*(Scalar a, const SWCSpectrum &s) {
		return s * a;
	}
	SWCSpectrum operator/(Scalar a) const {
		return *this * (1.f / a);
	}
	SWCSpectrum &operator/=(Scalar a) {
		Scalar inv = 1.f / a;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] *= inv;
		return *this;
	}
	void AddWeighted(Scalar w, const SWCSpectrum &s) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] += w * s.c[i];
	}
	bool operator==(const SWCSpectrum &sp) const {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			if (c[i] != sp.c[i]) return false;
		return true;
	}
	bool operator!=(const SWCSpectrum &sp) const {
		return !(*this == sp);
	}
	bool Black() const {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			if (c[i] != 0.f) return false;
		return true;
	}
	SWCSpectrum Sqrt() const {
		SWCSpectrum ret;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] = sqrtf(c[i]);
		return ret;
	}
	SWCSpectrum Pow(const SWCSpectrum &e) const {
		SWCSpectrum ret;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] = c[i] > 0 ? powf(c[i], e.c[i]) : 0.f;
		return ret;
	}
	SWCSpectrum operator-() const {
		SWCSpectrum ret;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] = -c[i];
		return ret;
	}
	friend SWCSpectrum Exp(const SWCSpectrum &s) {
		SWCSpectrum ret;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] = expf(s.c[i]);
		return ret;
	}
	SWCSpectrum Clamp(Scalar low = 0.f,
	               Scalar high = INFINITY) const {
		SWCSpectrum ret;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] = ::Clamp(c[i], low, high);
		return ret;
	}
	bool IsNaN() const {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			if (isnan(c[i])) return true;
		return false;
	}
	void Print(FILE *f) const {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			fprintf(f, "%f ", c[i]);
	}
	XYZColor ToXYZ(const TsPack *tspack) const;
	Scalar y(const TsPack *tspack) const;
	Scalar filter(const TsPack *tspack) const;

//	bool operator<(const SWCSpectrum &s2) const {
//		return y() < s2.y();												// Note - radiance - REFACT - need to rewrite without use of Spectrumwavelengths
//		return false;
//	}
	friend class girl::ParamSet;
	
	// SWCSpectrum Public Data
	Scalar c[WAVELENGTH_SAMPLES];
	
private:
	template<class Archive>
			void serialize(Archive & ar, const unsigned int version)
			{
				for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
					ar & c[i];
			}
};

}//namespace girl

#endif // GIRL_RGBColor_H
