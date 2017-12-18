/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   irregulardata.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - irregulardata.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// irregulardata.cpp*
#include "irregulardata.h"
#include "error.h"
#include "dynload.h"

using namespace girl;

// IrregularDataTexture Method Definitions
Texture<float> * IrregularDataTexture::CreateFloatTexture(const Transform &tex2world,
		const TextureParams &tp) {
	int wlCount;
	const float *wl = tp.FindFloats("wavelengths", &wlCount);
	int dataCount;
	const float *data = tp.FindFloats("data", &dataCount);
	if(wlCount != dataCount) {
		std::stringstream ss;
        ss << "Number of wavelengths '" << wlCount << "' does not match number of data values '"
			<< dataCount <<"' in irregular texture definition.";
        girlError(GIRL_BADTOKEN, GIRL_ERROR, ss.str().c_str());
	}
	return new IrregularDataFloatTexture<float>(1.f);
}

Texture<SWCSpectrum> * IrregularDataTexture::CreateSWCSpectrumTexture(const Transform &tex2world,
		const TextureParams &tp) {
	int wlCount;
	const float *wl = tp.FindFloats("wavelengths", &wlCount);
	int dataCount;
	const float *data = tp.FindFloats("data", &dataCount);
	if(wlCount != dataCount) {
		std::stringstream ss;
        ss << "Number of wavelengths '" << wlCount << "' does not match number of data values '"
			<< dataCount <<"' in irregular texture definition.";
        girlError(GIRL_BADTOKEN, GIRL_ERROR, ss.str().c_str());
	}
	return new IrregularDataSpectrumTexture<SWCSpectrum>(dataCount, wl, data);
}

static DynamicLoader::RegisterFloatTexture<IrregularDataTexture> r1("irregulardata");
static DynamicLoader::RegisterSWCSpectrumTexture<IrregularDataTexture> r2("irregulardata");
