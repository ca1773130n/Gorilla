/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   fleximage.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - fleximage.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_FLEXIMAGE_H
#define GIRL_FLEXIMAGE_H

#include "girl.h"
#include "film.h"
#include "color.h"
#include "paramset.h"
#include "tonemap.h"
#include "sampling.h"
#include <boost/thread/xtime.hpp>

namespace girl {

// FlexImageFilm Declarations
class GIRLCOREDLL  FlexImageFilm : public Film {
public:
	// FlexImageFilm Public Methods
	FlexImageFilm(int xres, int yres) :
		Film(xres, yres, 0), filter(NULL), filterTable(NULL),
		framebuffer(NULL), colorSpace(0.63f, 0.34f, 0.31f, 0.595f, 0.155f, 0.07f, 0.314275f, 0.329411f, 1.f) { }

	FlexImageFilm(int xres, int yres, Filter *filt, const float crop[4],
		const string &filename1, bool premult, int wI, int dI,
		bool w_tonemapped_EXR, bool w_untonemapped_EXR, bool w_tonemapped_IGI,
		bool w_untonemapped_IGI, bool w_tonemapped_TGA, bool w_resume_FLM, bool restart_resume_FLM,
		int haltspp, float reinhard_prescale, float reinhard_postscale,	float reinhard_burn, 
		const float cs_red[2], const float cs_green[2], const float cs_blue[2], const float whitepoint[2],
		float g, int reject_warmup, bool debugmode);
	~FlexImageFilm() {
		delete[] framebuffer;
	}

	int RequestBuffer(BufferType type, BufferOutputConfig output, const string& filePostfix);
	void CreateBuffers();

	void GetSampleExtent(int *xstart, int *xend, int *ystart, int *yend) const;
	void AddSample(Contribution *contrib);
	void AddSampleCount(float count, int bufferGroup = 0);

	void WriteImage(ImageType type);

	// GUI display methods
	void updateFrameBuffer();
	unsigned char* getFrameBuffer();
	void createFrameBuffer();
	float getldrDisplayInterval() {
		return displayInterval;
	}

	// Dade - method useful for transmitting the samples to a client
	void TransmitFilm(std::basic_ostream<char> &stream,
		int buf_id = 0, int bufferGroup = 0, bool clearBuffer = true);
	float UpdateFilm(Scene *scene, std::basic_istream<char> &stream,
		int buf_id = 0, int bufferGroup = 0);

	static Film *CreateFilm(const ParamSet &params, Filter *filter);

private:
	static void GetColorspaceParam(const ParamSet &params, const string name, float values[2]);

	void FlushSampleArray();
	// Dade - using this method requires to lock arrSampleMutex
	void MergeSampleArray();

	void WriteImage2(ImageType type, vector<Color> &color, vector<float> &alpha, string postfix);
	void WriteTGAImage(vector<Color> &rgb, vector<float> &alpha, const string &filename);
	void WriteEXRImage(vector<Color> &rgb, vector<float> &alpha, const string &filename);
	void WriteIGIImage(vector<Color> &rgb, vector<float> &alpha, const string &filename);
	void WriteResumeFilm(const string &filename);
	void ScaleOutput(vector<Color> &color, vector<float> &alpha, float *scale);

	// FlexImageFilm Private Data
	Filter *filter;
	int writeInterval;
	int displayInterval;
	string filename;
	bool premultiplyAlpha, buffersInited;
	float cropWindow[4], *filterTable;
	int xPixelStart, yPixelStart, xPixelCount, yPixelCount;
	ParamSet toneParams;
	float gamma;
	float reject_warmup_samples;
	bool writeTmExr, writeUtmExr, writeTmIgi, writeUtmIgi, writeTmTga, writeResumeFlm, restartResumeFlm;

	unsigned char *framebuffer;
	// Dade - timer is broken under Linux when using multiple threads, using
	// instead
	boost::xtime lastWriteImageTime;

	bool debug_mode;

	std::vector<BufferConfig> bufferConfigs;
	std::vector<BufferGroup> bufferGroups;

	float maxY;
	u_int warmupSamples;
	bool warmupComplete;
	ColorSystem colorSpace;
};

}//namespace girl

#endif //GIRL_FLEXIMAGE_H

