/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   blender_voronoi.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - blender_voronoi.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "blender_voronoi.h"
#include "dynload.h"

using namespace girl;
using namespace blender;

static DynamicLoader::RegisterFloatTexture<BlenderVoronoiTexture3D<float> > r1("blender_voronoi");
static DynamicLoader::RegisterSWCSpectrumTexture<BlenderVoronoiTexture3D<SWCSpectrum> > r2("blender_voronoi");
