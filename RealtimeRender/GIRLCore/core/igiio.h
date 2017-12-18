/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   igiio.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - igiio.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_IGIIO_H
#define GIRL_IGIIO_H
// igiio.h*

#include "girl.h"
#include "color.h"

namespace girl
{

// Based on code from GPL Violet Tonemapper.
static const int INDIGO_IMAGE_MAGIC_NUMBER = 66613373;

class GIRLCOREDLL  IndigoImageHeader
{
public:
	IndigoImageHeader() {};
	~IndigoImageHeader() {};

	//all integers and unsigned integers are 32 bit
	//Byte order should be little endian (Intel byte order)
	int magic_number;//should be 66613373
	int format_version;//1
	double num_samples;//total num samples taken over entire image
	unsigned int width;//width of supersampled (large) image
	unsigned int height;//height of supersampled (large) image
	unsigned int supersample_factor;// >= 1
	int zipped;//boolean

	int image_data_size;//size of image data in bytes
	//should be equal to width*height*12, if data is uncompressed.

	unsigned int colour_space;//0 = XYZ
	unsigned char padding[5000];//padding in case i want more stuff in the header in future

	//image data follows:
	//top row, then next-to-top row, etc...
	//left to right across the row.
	//3 32 bit floats per pixel.
};

void WriteIgiImage(const string &name,
	vector<Color> &pixels, vector<float> &alpha, int XRes, int YRes,
	int totalXRes, int totalYRes, int xOffset, int yOffset);

}

#endif // GIRL_IGIIO_H

