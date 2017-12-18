/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   error.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - error.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_ERROR_H
#define GIRL_ERROR_H

#include "api.h"

#include <sstream>
#include <string>

#define BOOST_ENABLE_ASSERT_HANDLER
#define BOOST_ENABLE_ASSERTS
//#define BOOST_DISABLE_ASSERTS
#include <boost/assert.hpp>

extern GIRLCOREDLL GirlErrorHandler girlError;

namespace boost
{
	inline void assertion_failed(char const *expr, char const *function, char const *file, long line)
	{
		std::ostringstream o;
		o<< "Assertion '"<<expr<<"' failed in function '"<<function<<"' (file:"<<file<<" line:"<<line<<")";
		girlError(GIRL_BUG, GIRL_SEVERE, const_cast<char *>(o.str().c_str()));
	}	
}


#endif //GIRL_ERROR_H
