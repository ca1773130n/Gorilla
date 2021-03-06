/***************************************************************************
 *   Copyright (C) 1998-2007 by authors (see AUTHORS.txt )                 *
 *                                                                         *
 *   This file is part of Lux Renderer.                                    *
 *                                                                         *
 *   Lux Renderer is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Lux Renderer is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *   This project is based on PBRT ; see http://www.pbrt.org               *
 *   Lux Renderer website : http://www.luxrender.org                       *
 ***************************************************************************/

// image.cpp*
#include "lux.h"
#include "film.h"
#include "color.h"
#include "paramset.h"
#include "tonemap.h"
#include "sampling.h"
#include <boost/timer.hpp>

namespace lux
{

// ImageFilm Declarations
class ImageFilm : public Film {
public:
	// ImageFilm Public Methods
	ImageFilm(int xres, int yres,
	                     Filter *filt, const float crop[4],
		             const string &filename, bool premult,
		             int wf, int wfs);
	~ImageFilm() {
		delete pixels;
		delete filter;
		delete[] filterTable;
	}
	void AddSample(float sX, float sY, 
	               const Spectrum &L, float alpha);
	void GetSampleExtent(int *xstart, int *xend,
	                     int *ystart, int *yend) const;
	void WriteImage();
	void WriteImage(int oType) {};

	unsigned char* getFrameBuffer() { return NULL; };
	void updateFrameBuffer() {};
	float getldrDisplayInterval() { return 10.0f; }
	
	static Film *CreateFilm(const ParamSet &params, Filter *filter);
private:
	// ImageFilm Private Data
	Filter *filter;
	int writeFrequency, writeFrequencySeconds, sampleCount;
	string filename;
	bool premultiplyAlpha;
	float cropWindow[4];
	int xPixelStart, yPixelStart, xPixelCount, yPixelCount;
	struct Pixel {
		Pixel() : L(0.f) {
			alpha = 0.f;
			weightSum = 0.f;
		}
		Spectrum L;
		float alpha, weightSum;
	};
	BlockedArray<Pixel> *pixels;
	float *filterTable;
	boost::timer Timer;
};

}//namespace lux


