/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   lowdiscrepancy.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - lowdiscrepancy.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// lowdiscrepancy.cpp*
#include "sampling.h"
#include "paramset.h"
#include "film.h"

namespace girl
{

// LDSampler Declarations
class GIRLCOREDLL  LDSampler : public Sampler {
public:
	// LDSampler Public Methods
	LDSampler(int xstart, int xend,
	          int ystart, int yend,
			  int nsamp, string pixelsampler);
	~LDSampler();

	int RoundSize(int size) const {
		return RoundUpPow2(size);
	}
	void GetBufferType(BufferType *type) {*type = BUF_TYPE_PER_PIXEL;}
	u_int GetTotalSamplePos();
	bool GetNextSample(Sample *sample, u_int *use_pos);
	float *GetLazyValues(Sample *sample, u_int num, u_int pos);
	virtual LDSampler* clone() const; // Girl (copy) constructor for multithreading

	static Sampler *CreateSampler(const ParamSet &params, const Film *film);
private:
	// LDSampler Private Data
	int xPos, yPos, pixelSamples;
	int samplePos;
	float *imageSamples, *lensSamples, *timeSamples, *wavelengthsSamples,
		*singleWavelengthSamples;
	float **oneDSamples, **twoDSamples, **xDSamples;
	int n1D, n2D, nxD;
	u_int TotalPixels;
	PixelSampler* pixelSampler;
};

}//namespace girl

