#pragma once
#include <stdio.h>
#include <time.h>
#ifdef __GNUC__
#include<stdint.h>
#endif
#include "common.h"
#include "gbUtilsConfig.h"

#define MAXTIMEBUFFERLEN 64

#ifdef _MSC_VER
typedef __int64 int64_t;
#endif

namespace gb
{
    namespace utils
    {
	gbUtilsEXPORT_CLASS time
	{
	    SingletonDeclare(time);
	public:
	    void get_local_time(char * const buffer, const unsigned int length = MAXTIMEBUFFERLEN);
	    int64_t get_local_time();
	    void format_time(int64_t rawTime, char * const buffer, const unsigned int length = MAXTIMEBUFFERLEN);
	};
#define GB_LocalTime(x)				\
	char x[MAXTIMEBUFFERLEN] = { 0 };	\
	time::Instance().get_local_time(x);

#define timeFormat(rawTime, buffer)			\
	char buffer[MAXTIMEBUFFERLEN] = { 0 };		\
	time::Instance().format_time(rawTime, buffer);
	
    };
};


