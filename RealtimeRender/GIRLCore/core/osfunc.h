/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   osfunc.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - osfunc.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_OSFUNC_H
#define GIRL_OSFUNC_H

#include <istream>
#include <ostream>
#include "girl.h"

namespace girl
{

// Dade - return the number of concurrent threads the hardware can run or
// 0 if the information is not avilable. The implementation come from Boost 1.35
extern GIRLCOREDLL int osHardwareConcurrency();

// Dade - used to check and swap bytes in the network rendering code and
// other places
extern bool osIsLittleEndian();
extern void osWriteLittleEndianFloat(bool isLittleEndian,
		std::basic_ostream<char> &os, float value);
extern void osReadLittleEndianFloat(bool isLittleEndian,
		std::basic_istream<char> &is, float *value);
extern void osWriteLittleEndianInt(bool isLittleEndian,
		std::basic_ostream<char> &os, int value);
extern void osReadLittleEndianInt(bool isLittleEndian,
		std::basic_istream<char> &is, int *value);

}//namespace girl

#endif // GIRL_OSFUNC_H
