/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   film.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - film.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_FILM_H
#define GIRL_FILM_H
// film.h*
#include "girl.h"
#include "color.h"
#include "error.h"
#include "memory.h"

#include <boost/serialization/split_member.hpp>

namespace girl {

enum ImageType {
    IMAGE_NONE = 0, // Don't write anything
    IMAGE_FILEOUTPUT = 1 << 1, // Write image to file
    IMAGE_FRAMEBUFFER = 1 << 2, // Display image
    IMAGE_ALL = IMAGE_FILEOUTPUT | IMAGE_FRAMEBUFFER
};

// Buffer types

enum BufferType {
    BUF_TYPE_PER_PIXEL = 0, // Per pixel normalized buffer
    BUF_TYPE_PER_SCREEN, // Per screen normalized buffer
    BUF_TYPE_RAW, // No normalization
    NUM_OF_BUFFER_TYPES
};

enum BufferOutputConfig {
    BUF_FRAMEBUFFER = 1 << 0, // Buffer is part of the rendered image
    BUF_STANDALONE = 1 << 1, // Buffer is written in its own file
    BUF_RAWDATA = 1 << 2 // Buffer data is written as is
};

class GIRLCOREDLL  BufferConfig {
public:
	BufferConfig(BufferType t, BufferOutputConfig o, const string& s) :
		type(t), output(o), postfix(s) { }
	BufferType type;
	BufferOutputConfig output;
	string postfix;
};

struct Pixel {
	// Dade - serialization here is required by network rendering
	friend class boost::serialization::access;

	template<class Archive> void serialize(Archive & ar, const unsigned int version) {
		ar & L;
		ar & alpha;
		ar & weightSum;
	}

	Pixel(): L(0.f), alpha(0.f), weightSum(0.f) { }
	XYZColor L;
	float alpha, weightSum;
};

class GIRLCOREDLL  Buffer {
public:
	Buffer(int x, int y) : xPixelCount(x), yPixelCount(y) {
		pixels = new BlockedArray<Pixel>(x, y);
	}

	virtual ~Buffer() {
		delete pixels; 
	}

	void Add(int x, int y, XYZColor L, float alpha, float wt) {
		Pixel &pixel = (*pixels)(x, y);
		pixel.L.AddWeighted(wt, L);
		pixel.alpha += alpha * wt;
		pixel.weightSum += wt;
	}

	void Clear() {
		for (int y = 0, offset = 0; y < yPixelCount; ++y) {
			for (int x = 0; x < xPixelCount; ++x, ++offset) {
				Pixel &pixel = (*pixels)(x, y);
				pixel.L.c[0] = 0.0f;
				pixel.L.c[1] = 0.0f;
				pixel.L.c[2] = 0.0f;
				pixel.alpha = 0.0f;
				pixel.weightSum = 0.0f;
			}
		}
	}

	virtual void GetData(float *rgb, float *alpha) const = 0;
	virtual void GetData(int x, int y, Color *color, float *alpha) const = 0;
	bool isFramebuffer;
	int xPixelCount, yPixelCount;
	float scaleFactor;
	BlockedArray<Pixel> *pixels;
};

// Per pixel normalized buffer
class RawBuffer : public Buffer {
public:
	RawBuffer(int x, int y) : Buffer(x, y) { }

	~RawBuffer() { }

	void GetData(float *rgb, float *alpha) const {
		for (int y = 0, offset = 0; y < yPixelCount; ++y) {
			for (int x = 0; x < xPixelCount; ++x, ++offset) {
				Pixel &pixel = (*pixels)(x, y);
				rgb[3*offset  ] = pixel.L.c[0];
				rgb[3*offset+1] = pixel.L.c[1];
				rgb[3*offset+2] = pixel.L.c[2];
				alpha[offset] = pixel.alpha;
			}
		}
	}
	void GetData(int x, int y, Color *color, float *alpha) const {
		*color = (*pixels)(x, y).L;
		*alpha = (*pixels)(x, y).alpha;
	}
};

// Per pixel normalized buffer
class PerPixelNormalizedBuffer : public Buffer {
public:
	PerPixelNormalizedBuffer(int x, int y) : Buffer(x, y) { }

	~PerPixelNormalizedBuffer() { }

	void GetData(float *rgb, float *alpha) const {
		for (int y = 0, offset = 0; y < yPixelCount; ++y) {
			for (int x = 0; x < xPixelCount; ++x, ++offset) {
				Pixel &pixel = (*pixels)(x, y);
				if (pixel.weightSum == 0.f) {
					alpha[offset] = 0.f;
					rgb[3*offset  ] = 0.f;
					rgb[3*offset+1] = 0.f;
					rgb[3*offset+2] = 0.f;
				} else {
					float inv = 1.f / pixel.weightSum;
					// Convert pixel XYZ radiance to RGB
					pixel.L.ToRGB(rgb + 3 * offset);
					rgb[3*offset  ] *= inv;
					rgb[3*offset+1] *= inv;
					rgb[3*offset+2] *= inv;
					alpha[offset] = pixel.alpha * inv;
				}
			}
		}
	}
	void GetData(int x, int y, Color *color, float *alpha) const {
		if ((*pixels)(x, y).weightSum == 0.f) {
			*color = XYZColor(0.f);
			*alpha = 0.f;
		} else {
			const float inv = 1.f / (*pixels)(x, y).weightSum;
			*color = (*pixels)(x, y).L * inv;
			*alpha = (*pixels)(x, y).alpha * inv;
		}
	}
};

// Per screen normalized  buffer
class PerScreenNormalizedBuffer : public Buffer {
public:
	PerScreenNormalizedBuffer(int x, int y, const double *samples) :
		Buffer(x, y), numberOfSamples_(samples) { }

	~PerScreenNormalizedBuffer() { }

	void GetData(float *rgb, float *alpha) const {
		const double inv = xPixelCount * yPixelCount / *numberOfSamples_;
		for (int y = 0, offset = 0; y < yPixelCount; ++y) {
			for (int x = 0; x < xPixelCount; ++x, ++offset) {
				Pixel &pixel = (*pixels)(x, y);
				// Convert pixel XYZ radiance to RGB
				pixel.L.ToRGB(rgb + 3 * offset);
				rgb[3*offset  ] *= inv;
				rgb[3*offset+1] *= inv;
				rgb[3*offset+2] *= inv;
				alpha[offset] = pixel.alpha * inv;
			}
		}
	}
	void GetData(int x, int y, Color *color, float *alpha) const {
		const double inv = xPixelCount * yPixelCount / *numberOfSamples_;
		*color = (*pixels)(x, y).L * inv;
		*alpha = (*pixels)(x, y).alpha * inv;
	}
private:
	const double *numberOfSamples_;
};


class BufferGroup {
public:
	BufferGroup() : numberOfSamples(0.f) { }
	~BufferGroup() {
		for(vector<Buffer *>::iterator buffer = buffers.begin(); buffer != buffers.end(); ++buffer)
			delete *buffer;
	}

	void CreateBuffers(const vector<BufferConfig> &configs, int x, int y) {
		for(vector<BufferConfig>::const_iterator config = configs.begin(); config != configs.end(); ++config) {
			switch ((*config).type) {
			case BUF_TYPE_PER_PIXEL:
				buffers.push_back(new PerPixelNormalizedBuffer(x, y));
				break;
			case BUF_TYPE_PER_SCREEN:
				buffers.push_back(new PerScreenNormalizedBuffer(x, y, &numberOfSamples));
				break;
			case BUF_TYPE_RAW:
				buffers.push_back(new RawBuffer(x, y));
				break;
			default:
				assert(0);
			}
		}
	}

	Buffer *getBuffer(int index) {
		return buffers[index];
	}
	double numberOfSamples;
	vector<Buffer *> buffers;
};

//class FlexImageFilm;

class GIRLCOREDLL  ArrSample {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & sX;
        ar & sY;
        ar & xyz;
        ar & alpha;
        ar & buf_id;
        ar & bufferGroup;
    }

public:
    void Sample() {
        sX = 0;
        sY = 0;
        xyz = XYZColor(0.);
        alpha = 0;
        buf_id = 0;
        bufferGroup = 0;
    }

    float sX;
    float sY;
    XYZColor xyz;
    float alpha;
    int buf_id;
    int bufferGroup;
};

// Film Declarations

class GIRLCOREDLL  Film {
public:
    // Film Interface

    Film(int xres, int yres, int haltspp) :
		xResolution(xres), yResolution(yres), haltSamplePerPixel(haltspp),
		enoughSamplePerPixel(false) {
		invSamplePerPass =  1.0 / (xResolution * yResolution);
	}
    virtual ~Film() { }
	virtual void AddSample(Contribution *contrib) = 0;
    virtual void AddSampleCount(float count, int bufferGroup = 0) { }
    virtual void WriteImage(ImageType type) = 0;
    virtual void GetSampleExtent(int *xstart, int *xend, int *ystart, int *yend) const = 0;

    virtual int RequestBuffer(BufferType type, BufferOutputConfig output, const string& filePostfix) {
        return 0;
    }

    virtual void CreateBuffers() {
    }
    virtual unsigned char* getFrameBuffer() = 0;
    virtual void updateFrameBuffer() = 0;
    virtual float getldrDisplayInterval() = 0;

    void SetScene(Scene *scene1) {
        scene = scene1;
    }

    // Film Public Data
    int xResolution, yResolution;

	// Dade - Samplers will check this flag to know if we have enough samples per
	// pixel and it is time to stop
	int haltSamplePerPixel;
	bool enoughSamplePerPixel;

	Scene *scene;

protected:
	// Dade - 1.0 / (xResolution * yResolution)
	double invSamplePerPass;
};

// Image Pipeline Declarations
extern void ApplyImagingPipeline(vector<Color> &pixels, int xResolution, int yResolution,
        ColorSystem &colorSpace, float bloomRadius = .2f,
        float bloomWeight = 0.f, const char *tonemap = NULL,
        const ParamSet *toneMapParams = NULL, float gamma = 2.2,
        float dither = 0.5f);

}//namespace girl;

#endif // GIRL_FILM_H
