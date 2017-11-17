#pragma once
#include <stdio.h>
#include <time.h>
#include "common.h"
#include "gbUtilsConfig.h"

#define GB_TIMEBUFFER_MAX_LEN 64

#define GB_GET_LOCALTIME(buffer)		\
    char buffer[GB_TIMEBUFFER_MAX_LEN] = { 0 };	\
    time::Instance().get_localtime(buffer);

namespace gb
{
    namespace utils
    {
	gbUtilsEXPORT_CLASS time
	{
	    GB_SINGLETON(time);
	public:
	    void get_localtime(char * const buffer, const unsigned char length = GB_TIMEBUFFER_MAX_LEN)const;
	    time_t get_localtime()const;
	};
    };
};


