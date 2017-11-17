#pragma once
#include <iostream>
#include <string>
#include "time.h" 

#ifdef gbLUAAPI
#include "luacpp.h"
#endif

#ifdef _MSC_VER
#include<Windows.h>

// colors are 0=black 1=blue 2=green and so on to 15=white  
// colorattribute = foreground + background * 16
// to get red text on yellow use 4 + 14*16 = 228
// light red on yellow would be 12 + 14*16 = 236
#define GB_LOGGER_LOG_MS_COLOR 15
#define GB_LOGGER_ERROR_MS_COLOR 12
#define GB_LOGGER_WARNING_MS_COLOR 14

#elif __GNUC__

// ref https://en.wikipedia.org/wiki/ANSI_escape_code#Windows_and_DOS
#define GB_LOGGER_COLOR_BEGIN "\033["
#define GB_LOGGER_COLOR_END "m"
#define GB_LOGGER_COLOR_BACKTONORMAL "\033[0m"

#endif

/*
 *TODO:
 *1.multi-threads support
 *2.permanent store
 */

#define _GB_DEFAULT_STREAMBUF_IDX 0
#define _GB_CURRENT_STREAMBUF_IDX 1

namespace gb
{
    namespace utils
    {
	gbUtilsEXPORT_CLASS  logger
	{
	    GB_SINGLETON_EXCLUDECTOR(logger);
	    logger();
	public:
	    void log(const char* msg)const;
	    void set_log_color_code(const char* szCode);
	    /*
	     *@param streambuf, if streambuf == nullptr,
	     *then set back to default streambuf.
	     */
	    void set_log_streambuf(std::streambuf* streambuf);
	    void error(const char* msg)const;
	    void set_error_color_code(const char* szCode);
	    void set_error_streambuf(std::streambuf* streambuf);
	    void warning(const char* msg)const;
	    void set_warning_color_code(const char* szCode);

	private:
	    std::string _log_color_code;
	    std::string _error_color_code;
	    std::string _warning_color_code;

	    /*
	     *log and waring share same streambuf
	     */
	    std::streambuf* _log_default_streambuf;
	    std::streambuf* _error_default_streambuf;
	    
#ifdef _MSC_VER
	    HANDLE _hConsole;
#endif
	    
#ifdef gbLUAAPI
	    gb_LC_EXPORT_FUNC(log);
	    gb_LC_EXPORT_FUNC(error);
	    gb_LC_EXPORT_FUNC(warning);
	    gb_LC_Register_PrvCns(log);
#endif

	};
	
    };
};
