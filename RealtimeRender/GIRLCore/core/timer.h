/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   timer.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - timer.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_TIMER_H
#define GIRL_TIMER_H
// timer.h*
#include "girl.h"
#if defined ( WIN32 ) || defined(__CYGWIN__)
#include <windows.h>
#else
#include <sys/time.h>
#endif
#if (_MSC_VER >= 1400)
#include <stdio.h>
#define snprintf _snprintf
#endif
// Timer Declarations
class GIRLCOREDLL   Timer {
public:
	// Public Timer Methods
	Timer();
	~Timer();
	
	void Start();
	void Stop();
	void Reset();
	
	double Time();
private:
	// Private Timer Data
	double time0, elapsed;
	bool running;
	double GetTime();
#if defined( IRIX ) || defined( IRIX64 )
	// Private IRIX Timer Data
	int fd;
	unsigned long long counter64;
	unsigned int counter32;
	unsigned int cycleval;
	
	typedef unsigned long long iotimer64_t;
	typedef unsigned int iotimer32_t;
	volatile iotimer64_t *iotimer_addr64;
	volatile iotimer32_t *iotimer_addr32;
	
	void *unmapLocation;
	int unmapSize;
#elif defined( WIN32 ) || defined(__CYGWIN__)
	// Private Windows Timer Data
	LARGE_INTEGER performance_counter, performance_frequency;
	double one_over_frequency;
#else
	// Private UNIX Timer Data
	struct timeval timeofday;
#endif
};
#endif // GIRL_TIMER_H
