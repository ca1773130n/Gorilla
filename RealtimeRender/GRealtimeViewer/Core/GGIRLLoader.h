#pragma once

#define NDEBUG 1

#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "girl.h"
#include "api.h"
#include "context.h"
#include "error.h"
#include "osfunc.h"

#if defined(WIN32) && !defined(__CYGWIN__) /* We need the following two to set stdout to binary */
#include <io.h>
#include <fcntl.h>
#endif

#if defined(WIN32) && !defined(__CYGWIN__)
#include "direct.h"
#define chdir _chdir
#endif

class CGGIRLLoader
{
public:
	//VOID	engineThread();

	//VOID	infoThread();

	VOID	Load( char* szFileName );

private:
	
};