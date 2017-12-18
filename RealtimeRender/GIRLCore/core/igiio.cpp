/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   igiio.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - igiio.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// igiio.cpp*
#include <algorithm>
#include <fstream>

#include "girl.h"
#include "error.h"
#include "color.h"
#include "spectrum.h"

#include "igiio.h"


using namespace girl;
// Indigo Image File Format IO - Based on code from GPL Violet Tonemapper.

namespace girl {
// IGI Function Definitions
 RGBColor *ReadIgiImage(const string &name, int *width, int *height) {
	// radiance - NOTE - unimplemented yet
		printf("IGI file format input not implemented yet");
		return NULL;
 }

 void WriteIgiImage(const string &name, vector<Color> &pixels,
		vector<float> &alpha, int xRes, int yRes,
		int totalXRes, int totalYRes,
		int xOffset, int yOffset) {

    IndigoImageHeader header;

	// create XYZ float buffer
    u_int xyzSize = xRes * yRes;
	float *xyz = new float[3 * xyzSize];
	for (u_int i = 0; i < xyzSize; ++i) {
		// convert RGB values to XYZ colour space.
		xyz[3 * i] = 0.436052025f * pixels[i].c[0] + 0.385081593f * pixels[i].c[1] + 0.143087414f * pixels[i].c[2];
		xyz[3 * i + 1] = 0.222491598f * pixels[i].c[0] + 0.71688606f * pixels[i].c[1] + 0.060621486f * pixels[i].c[2];
		xyz[3 * i + 2] = 0.013929122f * pixels[i].c[0] + 0.097097002f * pixels[i].c[1] + 0.71418547f  * pixels[i].c[2];
    }

	std::ofstream file(name.c_str(), std::ios::binary);
	if(!file) {
		std::stringstream ss;
	 	ss<< "Cannot open file '"<<name<<"' for output";
		girlError(GIRL_SYSTEM, GIRL_SEVERE, ss.str().c_str());
		return;
	}

	// set header
	memset(&header, 0, sizeof(header));
	header.magic_number = INDIGO_IMAGE_MAGIC_NUMBER;
	header.format_version = 1;

	header.num_samples = 1.; // TODO add samples from film
	header.width = xRes;
	header.height = yRes;
	header.supersample_factor = 1;
	header.zipped = false;
	header.image_data_size = xyzSize * 4;

	// write header
	file.write((const char*)&header, sizeof(header));
	// write xyz data
	file.write((const char*)&xyz[0], header.image_data_size);

	if(!file.good()) {
		std::stringstream ss;
	 	ss<< "Error writing IGI output file '"<<name<<"'";
		girlError(GIRL_SYSTEM, GIRL_SEVERE, ss.str().c_str());
		return;
	}

	file.close();
	delete xyz;
}

}
