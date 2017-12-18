/////////////// GIRL (Global Ilummination Renderer Library) Materials //////////////////
//	FileName:   girlmaterials.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2009-04-20  :   First version - girlmaterials.h initial implementation
//					DLL export/import directives
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "girl.h"
#include "mixmaterial.h"
#include "lunaShader.h"

// #ifdef GIRLMATERIAL_SOURCE
// #define  __declspec(dllexport)
// #else
// #define  __declspec(dllimport)
// #endif


// LunaShader classes
#include "lunaLambert.h"
#include "lunareflect.h"
#include "lunaBlinn.h"
#include "lunaPhong.h"
#include "lunaPhongE.h"