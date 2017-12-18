/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   random.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - random.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// random.cpp*
#include "sampling.h"
#include "paramset.h"
#include "film.h"

namespace girl
{

class GIRLCOREDLL  RandomSampler : public Sampler
{
public:
	RandomSampler(int xstart, int xend, int ystart, int yend,
		int ps, string pixelsampler);
	~RandomSampler();

	u_int GetTotalSamplePos();
	bool GetNextSample(Sample *sample, u_int *use_pos);
	float *GetLazyValues(Sample *sample, u_int num, u_int pos);
	int RoundSize(int sz) const
	{
		return sz;
	}
	void GetBufferType(BufferType *type) {*type = BUF_TYPE_PER_PIXEL;}
	virtual RandomSampler* clone() const; // Girl (copy) constructor for multithreading

	static Sampler *CreateSampler(const ParamSet &params, const Film *film);
private:
	// RandomSampler Private Data
	bool jitterSamples;
	int xPos, yPos, pixelSamples;
	float *imageSamples, *lensSamples, *timeSamples, *wavelengthsSamples,
		*singleWavelengthSamples;
	int samplePos;
	u_int TotalPixels;
	PixelSampler* pixelSampler;
	bool init;
};

}//namespace girl
