/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   harlequin.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - harlequin.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "harlequin.h"
#include "dynload.h"

using namespace girl;

static DynamicLoader::RegisterSWCSpectrumTexture<HarlequinTexture> r2("harlequin");
