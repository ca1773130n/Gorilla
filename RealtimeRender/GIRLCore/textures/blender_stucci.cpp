/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   blender_stucci.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - blender_stucci.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "blender_stucci.h"
#include "dynload.h"

using namespace girl;
using namespace blender;

static DynamicLoader::RegisterFloatTexture<BlenderStucciTexture3D<float> > r1("blender_stucci");
static DynamicLoader::RegisterSWCSpectrumTexture<BlenderStucciTexture3D<SWCSpectrum> > r2("blender_stucci");