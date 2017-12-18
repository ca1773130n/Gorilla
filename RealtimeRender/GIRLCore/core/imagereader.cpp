/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   imagereader.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - imagereader.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#include "girl.h"
#include "imagereader.h"
#include "error.h"
#define cimg_display_type  0

#ifdef GIRL_USE_CONFIG_H
#include "config.h"

#ifdef PNG_FOUND
#define cimg_use_png 1
#endif

#ifdef JPEG_FOUND
#define cimg_use_jpeg 1
#endif

#ifdef TIFF_FOUND
#define cimg_use_tiff 1
#endif


#else //GIRL_USE_CONFIG_H
#define cimg_use_png 1
#define cimg_use_tiff 1
#define cimg_use_jpeg 1
#endif //GIRL_USE_CONFIG_H


#define cimg_debug 0     // Disable modal window in CImg exceptions.
#include "cimg.h"
using namespace cimg_library;

#if defined(WIN32) && !defined(__CYGWIN__)
#define hypotf hypot // For the OpenEXR headers
#endif

#include <ImfInputFile.h>
#include <ImfOutputFile.h>
#include <ImfChannelList.h>
#include <ImfFrameBuffer.h>
#include <half.h>
using namespace Imf;
using namespace Imath;
using namespace girl;
