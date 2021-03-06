/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   mipmap.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - mipmap.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_MIPMAP_H
#define GIRL_MIPMAP_H

#include "girl.h"
#include "color.h"
#include "texture.h"
#include "texturecolor.h"
#include "memory.h"
#include "error.h"

namespace girl
{

// Dade - type of image filters
enum ImageTextureFilterType {
	NEAREST,
	BILINEAR,
	MIPMAP_TRILINEAR,
	MIPMAP_EWA
};

// MIPMap Declarations
typedef enum {
	TEXTURE_REPEAT,
	TEXTURE_BLACK,
	TEXTURE_CLAMP
} ImageWrap;

template <class T> class GIRLCOREDLL  MIPMap {
public:
	// MIPMap Public Methods
	virtual ~MIPMap() {};
	virtual T Lookup(float s, float t, float width = 0.f) const = 0;
	virtual T Lookup(float s, float t, float ds0, float dt0,
		float ds1, float dt1) const = 0;

	virtual u_int getMemoryUsed() const = 0;
	virtual void discardMipmaps(int n) { }
};

template <class T, class U> class GIRLCOREDLL  MIPMapImpl : public MIPMap<T> {
public:
	// MIPMapImpl Public Methods
	MIPMapImpl(
			ImageTextureFilterType type,
			int xres, int yres, const U *data,
			float maxAniso = 8.f, ImageWrap wrapMode = TEXTURE_REPEAT,
			float gain = 1.f, float gamma = 1.0f);
	~MIPMapImpl();

	T Lookup(float s, float t, float width = 0.f) const;
	T Lookup(float s, float t, float ds0, float dt0,
		float ds1, float dt1) const;

	u_int getMemoryUsed() const {
		if ((filterType == MIPMAP_EWA) || (filterType == MIPMAP_TRILINEAR)) {
			u_int size = 0;

			for (int i = 0; i < nLevels; i++)
				size += pyramid[i]->uSize() * pyramid[i]->vSize() * sizeof(U);

			return size;
		} else if ((filterType == NEAREST) || (filterType == BILINEAR)) {
			return singleMap->uSize() * singleMap->vSize() * sizeof(U);
		} else {
			girlError(GIRL_SYSTEM, GIRL_ERROR, "Internal error in MIPMapImpl::~MIPMapImpl(), unknown filter type");
			return 0;
		}
	}

	void discardMipmaps(int n) {
		for (int i = 0; i < n; i++) {
			if (nLevels <= 1)
				return;

			delete pyramid[0];

			nLevels--;
			BlockedArray<U> **newPyramid = new BlockedArray<U> *[nLevels];
			for (int j = 0; j < nLevels; j++)
				newPyramid[j] = pyramid[j + 1];

			delete[] pyramid;
			pyramid = newPyramid;
		}
	}

protected:
	// Dade - used by MIPMAP_EWA, MIPMAP_TRILINEAR
	const U& texelInternal(int level, int s, int t) const;
	const T texel(int level, int s, int t) const {
		T texelValue;
		convert(texelValue, texelInternal(level, s, t));

		return texelValue;
	}

	// Dade - used by NEAREST, BILINEAR
	const U& texelInternal(int s, int t) const;
	const T texel(int s, int t) const {
		T texelValue;
		convert(texelValue, texelInternal(s, t));

		return texelValue;
	}

	//template user specializations are not supported in vc++, so this hack
	//----------------------------- Unsigned Chars -----------------------------
	inline virtual void convert(RGBColor& outputValue, const TextureColor<unsigned char, 1> &internalValue) const
	{
		float c = static_cast<float>(internalValue.c[0]) * invMaxUnsignedChar;
		outputValue = (RGBColor(c) * gain).Pow(gamma);
	}
	inline virtual void convert(RGBColor& outputValue, const TextureColor<unsigned char, 3> &internalValue) const
	{
		float c[3];
		for (int i = 0; i < 3; ++i) {
			c[i] = static_cast<float>(internalValue.c[i]) * invMaxUnsignedChar;
		}
		outputValue = (RGBColor(c) * gain).Pow(gamma);
	}
	inline virtual void convert(RGBColor& outputValue, const TextureColor<unsigned char, 4> &internalValue) const
	{
		float c[3];
		for (int i = 0; i < 3; ++i) {
			c[i] = static_cast<float>(internalValue.c[i]) * invMaxUnsignedChar;
		}
		outputValue = (RGBColor(c) * gain).Pow(gamma);
	}

	//---------------------------- Unsigned Shorts -----------------------------
	inline virtual void convert(RGBColor& outputValue, const TextureColor<unsigned short, 1> &internalValue) const
	{
		float c = static_cast<float>(internalValue.c[0]) * invMaxUnsignedShort;
		outputValue = (RGBColor(c) * gain).Pow(gamma);
	}
	inline virtual void convert(RGBColor& outputValue, const TextureColor<unsigned short, 3> &internalValue) const
	{
		float c[3];
		for (int i = 0; i < 3; ++i) {
			c[i] = static_cast<float>(internalValue.c[i]) * invMaxUnsignedShort;
		}
		outputValue = (RGBColor(c) * gain).Pow(gamma);
	}
	inline virtual void convert(RGBColor& outputValue, const TextureColor<unsigned short, 4> &internalValue) const
	{
		float c[3];
		for (int i = 0; i < 3; ++i) {
			c[i] = static_cast<float>(internalValue.c[i]) * invMaxUnsignedShort;
		}
		outputValue= (RGBColor(c) * gain).Pow(gamma);
	}

	//--------------------------------- Floats ---------------------------------
	inline virtual void convert(RGBColor& outputValue, const TextureColor<float, 1> &internalValue) const
	{
		outputValue = (RGBColor(internalValue.c[0]) * gain).Pow(gamma);
	}
	inline virtual void convert(RGBColor& outputValue, const TextureColor<float, 3> &internalValue) const
	{
		outputValue = (RGBColor(internalValue.c) * gain).Pow(gamma);
	}
	inline virtual void convert(RGBColor& outputValue, const TextureColor<float, 4> &internalValue) const
	{
		outputValue = (RGBColor(internalValue.c) * gain).Pow(gamma);
	}
	inline virtual void convert(RGBColor& outputValue, const RGBColor &internalValue) const
	{
		outputValue = (internalValue * gain).Pow(gamma);
	}

	template<class V, int n>
	inline void convert(float& outputValue, const TextureColor<V, n> &internalValue) const
	{
		RGBColor specValue;
		convert(specValue, internalValue);
		outputValue = specValue.y();
	}
	inline virtual void convert(float& outputValue, const float internalValue) const
	{
		outputValue = powf(internalValue * gain, gamma);
	}

	// Dade - initialized by the class costructor
	static float invMaxUnsignedChar;
	static float invMaxUnsignedShort;

private:
	// MIPMAPImpl Provate data types
	struct ResampleWeight {
		int firstTexel;
		float weight[4];
	};
	// MIPMapImpl Private Methods
	ResampleWeight *resampleWeights(int oldres, int newres) {
		BOOST_ASSERT(newres >= oldres);
		ResampleWeight *wt = new ResampleWeight[newres];
		float filterwidth = 2.f;
		for (int i = 0; i < newres; ++i) {
			// Compute image resampling weights for _i_th texel
			float center = (i + .5f) * oldres / newres;
			wt[i].firstTexel = Floor2Int(center - filterwidth + 0.5f);
			for (int j = 0; j < 4; ++j) {
				float pos = wt[i].firstTexel + j + .5f;
				wt[i].weight[j] = Lanczos((pos - center) / filterwidth);
			}
			// Normalize filter weights for texel resampling
			float invSumWts = 1.f / (wt[i].weight[0] + wt[i].weight[1] +
				wt[i].weight[2] + wt[i].weight[3]);
			for (int j = 0; j < 4; ++j)
				wt[i].weight[j] *= invSumWts;
		}
		return wt;
	}

	inline int uSize(int level) const { return pyramid[level]->uSize(); }
	inline int vSize(int level) const { return pyramid[level]->vSize(); }

	T triangle(int level, float s, float t) const;
	T triangle(float s, float t) const;
	T nearest(float s, float t) const;
	T EWA(float s, float t, float ds0, float dt0, float ds1, float dt1, int level) const;

	// MIPMap Private Data
	ImageTextureFilterType filterType;
	float maxAnisotropy;
	float gain;
	float gamma;
	ImageWrap wrapMode;
	int nLevels;
	union {
		BlockedArray<U> **pyramid;
		BlockedArray<U> *singleMap;
	};

	#define WEIGHT_LUT_SIZE 128
	static float *weightLut;

	//equivalent of user specializations of template type
	inline float clamp(float v) { return max(v, 0.f); }
	inline RGBColor clamp(const RGBColor &v) { return v.Clamp(0.f, INFINITY); }
	inline TextureColor<unsigned char, 1> clamp(const TextureColor<unsigned char, 1> &v) { return v.Clamp(0.f, INFINITY); }
	inline TextureColor<unsigned char, 3> clamp(const TextureColor<unsigned char, 3> &v) { return v.Clamp(0.f, INFINITY); }
	inline TextureColor<unsigned char, 4> clamp(const TextureColor<unsigned char, 4> &v) { return v.Clamp(0.f, INFINITY); }
	inline TextureColor<unsigned short, 1> clamp(const TextureColor<unsigned short, 1> &v) { return v.Clamp(0.f, INFINITY); }
	inline TextureColor<unsigned short, 3> clamp(const TextureColor<unsigned short, 3> &v) { return v.Clamp(0.f, INFINITY); }
	inline TextureColor<unsigned short, 4> clamp(const TextureColor<unsigned short, 4> &v) { return v.Clamp(0.f, INFINITY); }
	inline TextureColor<float, 1> clamp(const TextureColor<float, 1> &v) { return v.Clamp(0.f, INFINITY); }
	inline TextureColor<float, 3> clamp(const TextureColor<float, 3> &v) { return v.Clamp(0.f, INFINITY); }
	inline TextureColor<float, 4> clamp(const TextureColor<float, 4> &v) { return v.Clamp(0.f, INFINITY); }
};

template <class T, class U> float *MIPMapImpl<T, U>::weightLut = NULL;
template <class T, class U> float MIPMapImpl<T, U>::invMaxUnsignedChar = 1.0f / (std::numeric_limits<unsigned char>::max() - 1);
template <class T, class U> float MIPMapImpl<T, U>::invMaxUnsignedShort = 1.0f / (std::numeric_limits<unsigned short>::max() - 1);

// MIPMapImpl Method Definitions

template <class T, class U>
T MIPMapImpl<T, U>::Lookup(float s, float t, float width) const
{
	switch (filterType) {
		case MIPMAP_TRILINEAR:
		case MIPMAP_EWA: {
			// Compute MIPMap level for trilinear filtering
			float level = nLevels - 1 + Log2(max(width, 1e-8f));
			// Perform trilinear interpolation at appropriate MIPMap level
			if (level < 0)
				return triangle(0, s, t);
			else if (level >= nLevels - 1)
				return texel(nLevels - 1, 0, 0);
			else {
				int iLevel = Floor2Int(level);
				float delta = level - iLevel;
				return Lerp<T>(delta, triangle(iLevel, s, t), triangle(iLevel + 1, s, t));
			}
		}
		case BILINEAR:
			return triangle(s, t);
		case NEAREST:
			return nearest(s, t);
		default:
			girlError(GIRL_SYSTEM, GIRL_ERROR, "Internal error in MIPMapImpl::Lookup()");
			return T();
	}
}

template <class T, class U>
T MIPMapImpl<T, U>::triangle(int level, float s, float t) const {
	level = Clamp(level, 0, nLevels - 1);
	s = s * uSize(level) - 0.5f;
	t = t * vSize(level) - 0.5f;
	int s0 = Floor2Int(s), t0 = Floor2Int(t);
	float ds = s - s0, dt = t - t0;
	return (1.f - ds) * (1.f - dt) * texel(level, s0, t0) +
		(1.f - ds) * dt * texel(level, s0, t0 + 1) +
		ds * (1.f - dt) * texel(level, s0 + 1, t0) +
		ds * dt * texel(level, s0 + 1, t0 + 1);
}

template <class T, class U>
T MIPMapImpl<T, U>::triangle(float s, float t) const {
	s = s * singleMap->uSize() - 0.5f;
	t = t * singleMap->vSize() - 0.5f;
	int s0 = Floor2Int(s), t0 = Floor2Int(t);
	float ds = s - s0, dt = t - t0;
	return (1.f - ds) * (1.f - dt) * texel(s0, t0) +
		(1.f - ds) * dt * texel(s0, t0 + 1) +
		ds * (1.f - dt) * texel(s0 + 1, t0) +
		ds * dt * texel(s0 + 1, t0 + 1);
}

template <class T, class U>
T MIPMapImpl<T, U>::nearest(float s, float t) const {
	s = s * singleMap->uSize() - 0.5f;
	t = t * singleMap->vSize() - 0.5f;
	int s0 = Floor2Int(s), t0 = Floor2Int(t);
	
	return texel(s0,t0);
}
			
template <class T, class U>
T MIPMapImpl<T, U>::Lookup(float s, float t, float ds0, float dt0,
	float ds1, float dt1) const
{
	switch (filterType) {
		case MIPMAP_TRILINEAR:
			return Lookup(s, t, 2.f * max(max(fabsf(ds0), fabsf(dt0)),
				max(fabsf(ds1), fabsf(dt1))));
		case MIPMAP_EWA: {
			// Compute ellipse minor and major axes
			if (ds0 * ds0 + dt0 * dt0 < ds1 * ds1 + dt1 * dt1) {
				swap(ds0, ds1);
				swap(dt0, dt1);
			}
			float majorLength = sqrtf(ds0 * ds0 + dt0 * dt0);
			float minorLength = sqrtf(ds1 * ds1 + dt1 * dt1);

			// Clamp ellipse eccentricity if too large
			if (minorLength * maxAnisotropy < majorLength) {
				float scale = majorLength / (minorLength * maxAnisotropy);
				ds1 *= scale;
				dt1 *= scale;
				minorLength *= scale;
			}

			// Choose level of detail for EWA lookup and perform EWA filtering
			float lod = max(0.f, nLevels - 1 + Log2(minorLength));
			int ilod = Floor2Int(lod);
			float d = lod - ilod;
			return Lerp<T>(d, EWA(s, t, ds0, dt0, ds1, dt1, ilod), EWA(s, t, ds0, dt0, ds1, dt1, ilod + 1));
		}
		case BILINEAR:
			return triangle(s, t);
		case NEAREST:
			return nearest(s, t);
		default:
			girlError(GIRL_SYSTEM, GIRL_ERROR, "Internal error in MIPMapImpl::Lookup()");
			return T();
	}
}

template <class T, class U>
T MIPMapImpl<T, U>::EWA(float s, float t, float ds0, float dt0,
	float ds1, float dt1, int level) const
{
	if (level >= nLevels)
		return texel(nLevels - 1, 0, 0);
	// Convert EWA coordinates to appropriate scale for level
	s = s * uSize(level) - 0.5f;
	t = t * vSize(level) - 0.5f;
	ds0 *= uSize(level);
	dt0 *= vSize(level);
	ds1 *= uSize(level);
	dt1 *= vSize(level);
	// Compute ellipse coefficients to bound EWA filter region
	float A = dt0 * dt0 + dt1 * dt1 + 1.;
	float B = -2.f * (ds0 * dt0 + ds1 * dt1);
	float C = ds0 * ds0 + ds1 * ds1 + 1.;
	float F = A * C - B * B * 0.25f;
	// Compute the ellipse's $(s,t)$ bounding box in texture space
	float du = sqrt(C), dv = sqrt(A);
	int s0 = Ceil2Int(s - du);
	int s1 = Floor2Int(s + du);
	int t0 = Ceil2Int(t - dv);
	int t1 = Floor2Int(t + dv);

	float invF = 1.0f / F;
	A *= invF;
	B *= invF;
	C *= invF;
	// radiance - disabled for threading // static StatsRatio ewaTexels("Texture", "Texels per EWA lookup"); // NOBOOK
	// radiance - disabled for threading // ewaTexels.Add((1+s1-s0) * (1+t1-t0), 1); // NOBOOK
	// Scan over ellipse bound and compute quadratic equation
	T num(0.f);
	float den = 0.f;
	for (int it = t0; it <= t1; ++it) {
		float tt = it - t;
		for (int is = s0; is <= s1; ++is) {
			float ss = is - s;
			// Compute squared radius and filter texel if inside ellipse
			float r2 = A * ss * ss + B * ss * tt + C * tt * tt;
			if (r2 < 1.) {
				float weight =
					weightLut[min(Float2Int(r2 * WEIGHT_LUT_SIZE),
					WEIGHT_LUT_SIZE - 1)];
				num += texel(level, is, it) * weight;
				den += weight;
			}
		}
	}

	return num / den;
}

template <class T,class U>
MIPMapImpl<T,U>::~MIPMapImpl() {
	if ((filterType == MIPMAP_EWA) || (filterType == MIPMAP_TRILINEAR)) {
		for (int i = 0; i < nLevels; ++i)
			delete pyramid[i];
		delete[] pyramid;
	} else if ((filterType == NEAREST) || (filterType == BILINEAR)) {
		delete singleMap;
	} else {
		girlError(GIRL_SYSTEM, GIRL_ERROR, "Internal error in MIPMapImpl::~MIPMapImpl(), unknown filter type");
	}
}

template <class T,class U>
MIPMapImpl<T,U>::MIPMapImpl(
		ImageTextureFilterType type, int sres, int tres,
		const U *img, float maxAniso, ImageWrap wm,
		float gn,float gma) {
	filterType = type;
	maxAnisotropy = maxAniso;
	wrapMode = wm;
	gain=gn;
	gamma=gma;

	if ((filterType == MIPMAP_EWA) || (filterType == MIPMAP_TRILINEAR)) {
		U *resampledImage = NULL;
		if (!IsPowerOf2(sres) || !IsPowerOf2(tres)) {
			// Resample image to power-of-two resolution
			int sPow2 = RoundUpPow2(sres), tPow2 = RoundUpPow2(tres);
			std::stringstream ss;
			ss << "Resampling image from " << sres << "x" << tres <<
					" to " << sPow2 << "x" << tPow2;
			girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

			// Resample image in $s$ direction
			struct ResampleWeight *sWeights = resampleWeights(sres, sPow2);
			resampledImage = new U[sPow2 * tPow2];

			// Apply _sWeights_ to zoom in $s$ direction
			for (int t = 0; t < tres; ++t) {
				for (int s = 0; s < sPow2; ++s) {
					// Compute texel $(s,t)$ in $s$-zoomed image
					resampledImage[t*sPow2+s] = U();
					// NOTE - Ratow - Offsetting weights to minimize possible over/underflows
					for (int jo = 2; jo < 6; ++jo) {
						int j = jo % 4;

						int origS = sWeights[s].firstTexel + j;
						if (wrapMode == TEXTURE_REPEAT)
							origS = Mod(origS, sres);
						else if (wrapMode == TEXTURE_CLAMP)
							origS = Clamp(origS, 0, sres - 1);

						if (origS >= 0 && origS < sres) {
							if(sWeights[s].weight[j] > 0.)
								resampledImage[t * sPow2 + s] += sWeights[s].weight[j] * img[t * sres + origS];
							else /* TextureColor cannot be negative so we invert and subtract */
								resampledImage[t * sPow2 + s] -= (-sWeights[s].weight[j]) * img[t * sres + origS];
						}
					}
				}
			}
			delete[] sWeights;

			// Resample image in $t$ direction
			struct ResampleWeight *tWeights = resampleWeights(tres, tPow2);
			U *workData = new U[tPow2];
			for (int s = 0; s < sPow2; ++s) {
				for (int t = 0; t < tPow2; ++t) {
					workData[t] = U();
					// NOTE - Ratow - Offsetting weights to minimize possible over/underflows
					for (int jo = 2; jo < 6; ++jo) {
						int j = jo % 4;

						int offset = tWeights[t].firstTexel + j;
						if (wrapMode == TEXTURE_REPEAT)
							offset = Mod(offset, tres);
						else if (wrapMode == TEXTURE_CLAMP)
							offset = Clamp(offset, 0, tres - 1);

						if (offset >= 0 && offset < tres) {
							if(tWeights[t].weight[j] > 0.)
								workData[t] += tWeights[t].weight[j] * resampledImage[offset * sPow2 + s];
							else /* TextureColor cannot be negative so we invert and subtract */
								workData[t] -= (-tWeights[t].weight[j]) * resampledImage[offset * sPow2 + s];
						}
					}
				}
				for (int t = 0; t < tPow2; ++t)
					resampledImage[t * sPow2 + s] = clamp(workData[t]);
			}
			delete[] workData;
			delete[] tWeights;
			img = resampledImage;  // XXX need to delete resampledImage when done...

			sres = sPow2;
			tres = tPow2;
		}

		// Initialize levels of MIPMap from image
		nLevels = 1 + Log2Int(max(sres, tres));

		std::stringstream ss;
		ss << "Generating " << nLevels << " mipmap levels";
		girlError(GIRL_NOERROR, GIRL_INFO, ss.str().c_str());

		pyramid = new BlockedArray<U> *[this->nLevels];
		// Initialize most detailed level of MIPMap
		pyramid[0] = new BlockedArray<U>(sres, tres, img);
		for (int i = 1; i < nLevels; ++i) {
			// Initialize $i$th MIPMap level from $i-1$st level
			int sRes = max(1, pyramid[i - 1]->uSize() / 2);
			int tRes = max(1, pyramid[i - 1]->vSize() / 2);
			pyramid[i] = new BlockedArray<U>(sRes, tRes);
			// Filter four texels from finer level of pyramid
			for (int t = 0; t < tRes; ++t) {
				for (int s = 0; s < sRes; ++s) {
					/* NOTE - Ratow - multiplying before summing all TextureColors because they can overflow */
					(*pyramid[i])(s, t) =
						0.25f * texelInternal(i - 1, 2 * s, 2 * t) +
						0.25f * texelInternal(i - 1, 2 * s + 1, 2 * t) +
						0.25f * texelInternal(i - 1, 2 * s, 2 * t + 1) +
						0.25f * texelInternal(i - 1, 2 * s + 1, 2 * t + 1);
				}
			}
		}
		if (resampledImage)
			delete[] resampledImage;

		// Initialize EWA filter weights if needed
		if (!weightLut) {
			weightLut = (float *)AllocAligned(WEIGHT_LUT_SIZE * sizeof(float));
			for (int i = 0; i < WEIGHT_LUT_SIZE; ++i) {
				float alpha = 2.;
				float r2 = static_cast<float>(i) / static_cast<float>(WEIGHT_LUT_SIZE - 1);
				weightLut[i] = expf(-alpha * r2) - expf(-alpha);
			}
		}
	} else if ((filterType == NEAREST) || (filterType == BILINEAR)) {
		singleMap = new BlockedArray<U>(sres, tres, img);
	} else {
		girlError(GIRL_SYSTEM, GIRL_ERROR, "Internal error in MIPMapImpl::MIPMapImpl(), unknown filter type");
	}
}

template <class T, class U>
const U& MIPMapImpl<T, U>::texelInternal(int level, int s, int t) const
{
	static const U black = U();
	const BlockedArray<U> &l = *pyramid[level];
	// Compute texel $(s,t)$ accounting for boundary conditions
	switch (wrapMode) {
		case TEXTURE_REPEAT:
			s = Mod(s, l.uSize());
			t = Mod(t, l.vSize());
			break;
		case TEXTURE_CLAMP:
			s = Clamp(s, 0, l.uSize() - 1);
			t = Clamp(t, 0, l.vSize() - 1);
			break;
		case TEXTURE_BLACK:
			if (s < 0 || s >= l.uSize() ||
				t < 0 || t >= l.vSize())
			return black;
	}

	return l(s, t);
}

template <class T, class U>
const U& MIPMapImpl<T, U>::texelInternal(int s, int t) const
{
	static const U black = U();
	const BlockedArray<U> &l = *singleMap;
	// Compute texel $(s,t)$ accounting for boundary conditions
	switch (wrapMode) {
		case TEXTURE_REPEAT:
			s = Mod(s, l.uSize());
			t = Mod(t, l.vSize());
			break;
		case TEXTURE_CLAMP:
			s = Clamp(s, 0, l.uSize() - 1);
			t = Clamp(t, 0, l.vSize() - 1);
			break;
		case TEXTURE_BLACK:
			if (s < 0 || s >= l.uSize() ||
				t < 0 || t >= l.vSize())
			return black;
	}

	return l(s, t);
}

// Dade - optimized path for gain = 1.0 and gamma = 1.0
template <class T, class U> class GIRLCOREDLL  MIPMapFastImpl : public MIPMapImpl<T, U> {
public:
	// MIPMapFastImpl Public Methods
	MIPMapFastImpl(ImageTextureFilterType type, int xres, int yres,
			const U *data, float maxAniso = 8.f, ImageWrap wrapMode = TEXTURE_REPEAT) :
		MIPMapImpl<T, U>(type, xres, yres, data, maxAniso, wrapMode, 1.0f, 1.0f) { };

private:
	//template user specializations are not supported in vc++, so this hack
	//----------------------------- Unsigned Chars -----------------------------
	inline void convert(RGBColor& outputValue, const TextureColor<unsigned char, 1> &internalValue) const
	{
		float c = static_cast<float>(internalValue.c[0]) * MIPMapImpl<T, U>::invMaxUnsignedChar;
		outputValue = RGBColor(c);
	}
	inline void convert(RGBColor& outputValue, const TextureColor<unsigned char, 3> &internalValue) const
	{
		float c[3];
		for (int i = 0; i < 3; ++i) {
			c[i] = static_cast<float>(internalValue.c[i]) * MIPMapImpl<T, U>::invMaxUnsignedChar;
		}
		outputValue = RGBColor(c);
	}
	inline void convert(RGBColor& outputValue, const TextureColor<unsigned char, 4> &internalValue) const
	{
		float c[3];
		for (int i = 0; i < 3; ++i) {
			c[i] = static_cast<float>(internalValue.c[i]) * MIPMapImpl<T, U>::invMaxUnsignedChar;
		}
		outputValue = RGBColor(c);
	}

	//---------------------------- Unsigned Shorts -----------------------------
	inline void convert(RGBColor& outputValue, const TextureColor<unsigned short, 1> &internalValue) const
	{
		float c = static_cast<float>(internalValue.c[0]) * MIPMapImpl<T, U>::invMaxUnsignedShort;
		outputValue = RGBColor(c);
	}
	inline void convert(RGBColor& outputValue, const TextureColor<unsigned short, 3> &internalValue) const
	{
		float c[3];
		for (int i = 0; i < 3; ++i) {
			c[i] = static_cast<float>(internalValue.c[i]) * MIPMapImpl<T, U>::invMaxUnsignedShort;
		}
		outputValue = RGBColor(c);
	}
	inline void convert(RGBColor& outputValue, const TextureColor<unsigned short, 4> &internalValue) const
	{
		float c[3];
		for (int i = 0; i < 3; ++i) {
			c[i] = static_cast<float>(internalValue.c[i]) * MIPMapImpl<T, U>::invMaxUnsignedShort;
		}
		outputValue = RGBColor(c);
	}

	//--------------------------------- Floats ---------------------------------
	inline void convert(RGBColor& outputValue, const TextureColor<float, 1> &internalValue) const
	{
		outputValue = RGBColor(internalValue.c[0]);
	}
	inline void convert(RGBColor& outputValue, const TextureColor<float, 3> &internalValue) const
	{
		outputValue = RGBColor(internalValue.c);
	}
	inline void convert(RGBColor& outputValue, const TextureColor<float, 4> &internalValue) const
	{
		outputValue = RGBColor(internalValue.c);
	}
	inline void convert(RGBColor& outputValue, const RGBColor &internalValue) const
	{
		outputValue = RGBColor(internalValue.c);
	}

	inline void convert(float& outputValue, const float internalValue) const
	{
		outputValue = internalValue;
	}
};

}//namespace girl

#endif // GIRL_MIPMAP_H
