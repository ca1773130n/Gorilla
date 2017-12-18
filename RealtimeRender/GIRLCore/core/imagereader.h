/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   imagereader.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - imagereader.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_IMAGEREADER_H
#define GIRL_IMAGEREADER_H
#include "girl.h"
#include "texturecolor.h"
#include "mipmap.h"
#include "error.h"
using namespace std;

namespace girl
{

class GIRLCOREDLL  ImageData {
public:
	enum PixelDataType {
		UNSIGNED_CHAR_TYPE,
		UNSIGNED_SHORT_TYPE,
		FLOAT_TYPE
	};

	ImageData(int width, int height, PixelDataType type, int noChannels, TextureColorBase* data) {
		width_ = width;
		height_ = height;
		pixel_type_ = type;
		noChannels_ = noChannels;
		// NOTE - Ratow - Not using auto_ptr here because of undefined behavior when deleting array data
		data_ = data;
		isExrImage_ = false;

	}

	~ImageData() {
		delete[] data_;
	}

	int getWidth() {
		return width_;
	}

	int getHeight() {
		return height_;
	}

	int getChannels() {
		return noChannels_;
	}

	PixelDataType getPixelDataType() {
		return pixel_type_;
	}

	TextureColorBase * getData() {
		return data_;
	}

	bool isExrImage() {
		return isExrImage_;
	}

	void setIsExrImage(bool isExrImage) {
		isExrImage_ = isExrImage;
	}

	template <class T> MIPMap<T> *createMIPMap(
			ImageTextureFilterType filterType = BILINEAR,
			float maxAniso = 8.f, ImageWrap wrapMode = TEXTURE_REPEAT,
			float gain = 1.0f, float gamma = 1.0f) {
		MIPMap<T> *mipmap = NULL;

		// Dade - added support for 1 channel maps
		if (noChannels_ == 1) {
			if (pixel_type_ == UNSIGNED_CHAR_TYPE) {
				if ((gain == 1.0f) && (gamma == 1.0f))
					mipmap = new MIPMapFastImpl<T, TextureColor<unsigned char, 1 > >(
							filterType, width_, height_,
							(TextureColor<unsigned char, 1 > *)data_,
							maxAniso, wrapMode);
				else
					mipmap = new MIPMapImpl<T, TextureColor<unsigned char, 1 > >(
							filterType, width_, height_,
							(TextureColor<unsigned char, 1 > *)data_,
							maxAniso, wrapMode, gain, gamma);
			} else if (pixel_type_ == FLOAT_TYPE) {
				if ((gain == 1.0f) && (gamma == 1.0f))
					mipmap = new MIPMapFastImpl<T, TextureColor<float, 1 > >(
							filterType, width_, height_,
							(TextureColor<float, 1 > *)data_,
							maxAniso, wrapMode);
				else
					mipmap = new MIPMapImpl<T, TextureColor<float, 1 > >(
							filterType, width_, height_,
							(TextureColor<float, 1 > *)data_,
							maxAniso, wrapMode, gain, gamma);
			} else if (pixel_type_ == UNSIGNED_SHORT_TYPE) {
				if ((gain == 1.0f) && (gamma == 1.0f))
					mipmap = new MIPMapFastImpl<T, TextureColor<unsigned short, 1 > >(
							filterType, width_, height_,
							(TextureColor<unsigned short, 1 > *)data_,
							maxAniso, wrapMode);
				else
					mipmap = new MIPMapImpl<T, TextureColor<unsigned short, 1 > >(
							filterType, width_, height_,
							(TextureColor<unsigned short, 1 > *)data_,
							maxAniso, wrapMode, gain, gamma);
			}
		} else if (noChannels_ == 3) {
			if (pixel_type_ == UNSIGNED_CHAR_TYPE) {
				if ((gain == 1.0f) && (gamma == 1.0f)) {
					mipmap = new MIPMapFastImpl<T, TextureColor<unsigned char, 3 > >(
							filterType, width_, height_,
							(TextureColor<unsigned char, 3 > *)data_,
							maxAniso, wrapMode);
				} else
					mipmap = new MIPMapImpl<T, TextureColor<unsigned char, 3 > >(
							filterType, width_, height_,
							(TextureColor<unsigned char, 3 > *)data_,
							maxAniso, wrapMode, gain, gamma);
			} else if (pixel_type_ == FLOAT_TYPE) {
				if ((gain == 1.0f) && (gamma == 1.0f))
					mipmap = new MIPMapFastImpl<T, TextureColor<float, 3 > >(
							filterType, width_, height_,
							(TextureColor<float, 3 > *)data_,
							maxAniso, wrapMode);
				else
					mipmap = new MIPMapImpl<T, TextureColor<float, 3 > >(
							filterType, width_, height_,
							(TextureColor<float, 3 > *)data_,
							maxAniso, wrapMode, gain, gamma);
			} else if (pixel_type_ == UNSIGNED_SHORT_TYPE) {
				if ((gain == 1.0f) && (gamma == 1.0f))
					mipmap = new MIPMapFastImpl<T, TextureColor<unsigned short, 3 > >(
							filterType, width_, height_,
							(TextureColor<unsigned short, 3 > *)data_,
							maxAniso, wrapMode);
				else
					mipmap = new MIPMapImpl<T, TextureColor<unsigned short, 3 > >(
							filterType, width_, height_,
							(TextureColor<unsigned short, 3 > *)data_,
							maxAniso, wrapMode, gain, gamma);
			}
		} else if (noChannels_ == 4) {
			if (pixel_type_ == UNSIGNED_CHAR_TYPE) {
				if ((gain == 1.0f) && (gamma == 1.0f))
					mipmap = new MIPMapFastImpl<T, TextureColor<unsigned char, 4 > >(
							filterType, width_, height_,
							(TextureColor<unsigned char, 4 > *)data_,
							maxAniso, wrapMode);
				else
					mipmap = new MIPMapImpl<T, TextureColor<unsigned char, 4 > >(
							filterType, width_, height_,
							(TextureColor<unsigned char, 4 > *)data_,
							maxAniso, wrapMode, gain, gamma);
			} else if (pixel_type_ == FLOAT_TYPE) {
				if ((gain == 1.0f) && (gamma == 1.0f))
					mipmap = new MIPMapFastImpl<T, TextureColor<float, 4 > >(
							filterType, width_, height_,
							(TextureColor<float, 4 > *)data_,
							maxAniso, wrapMode);
				else
					mipmap = new MIPMapImpl<T, TextureColor<float, 4 > >(
							filterType, width_, height_,
							(TextureColor<float, 4 > *)data_,
							maxAniso, wrapMode, gain, gamma);
			} else if (pixel_type_ == UNSIGNED_SHORT_TYPE) {
				if ((gain == 1.0f) && (gamma == 1.0f))
					mipmap = new MIPMapFastImpl<T, TextureColor<unsigned short, 4 > >(
							filterType, width_, height_,
							(TextureColor<unsigned short, 4 > *)data_,
							maxAniso, wrapMode);
				else
					mipmap = new MIPMapImpl<T, TextureColor<unsigned short, 4 > >(
							filterType, width_, height_,
							(TextureColor<unsigned short, 4 > *)data_,
							maxAniso, wrapMode, gain, gamma);
			}
		} else {
			girlError(GIRL_SYSTEM, GIRL_ERROR, "Unsupported channel count in ImageData::createMIPMap()");

			return NULL;
		}

		return mipmap;
	}

private:
	int width_;
	int height_;
	int noChannels_;
	TextureColorBase *data_;
	PixelDataType pixel_type_;
	bool isExrImage_;
};

class GIRLCOREDLL  ImageReader {
public:

	virtual ~ImageReader() {
	}
	virtual ImageData* read(const string &name) = 0;
};

class GIRLCOREDLL  ExrImageReader : public ImageReader {
public:

	virtual ~ExrImageReader() {
	}

	ExrImageReader() {
	};
	ImageData* read(const string &name);
};

}
#endif // GIRL_IMAGEREADER_H
