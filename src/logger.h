#pragma once
#include <iostream>
#include <string>
#include "time.h" 

#ifdef gbLUAAPI
#include "luacpp.h"
#endif

#ifdef WIN32
#include<Windows.h>
#endif
// colors are 0=black 1=blue 2=green and so on to 15=white  
// colorattribute = foreground + background * 16
// to get red text on yellow use 4 + 14*16 = 228
// light red on yellow would be 12 + 14*16 = 236
#define GB_LOG_LOG_COLOR 15
#define GB_LOG_ERROR_COLOR 12
#define GB_LOG_WARNING_COLOR 14

#define GB_LOG_MAX_LEN 256

namespace gb
{
    namespace utils
    {
	enum log_type
	{
	    Log = 1,
	    Warning, Error
	};

	gbUtilsEXPORT_CLASS  logger
	{
	    SingletonDeclare(logger);
	public:
	    void log(const char* msg);
	    void error(const char* msg);
	    void warning(const char* msg);
  
#ifdef gbLUAAPI
	    gb_LC_EXPORT_FUNC(log);
	    gb_LC_EXPORT_FUNC(error);
	    gb_LC_EXPORT_FUNC(warning);
	    gb_LC_Register_PrvCns(log);
#endif
	private:
#ifdef WIN32
	    static HANDLE hConsole;
#endif
	};

	
    };
};
