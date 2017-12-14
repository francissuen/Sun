#pragma once
#include <stdio.h>
#include <ctime>
#include "common.h"
#include "config.h"
#include "string.h"
#define GB_TIMEBUFFER_MAX_LEN 64

#define GB_GET_LOCALTIME(buffer)		\
    char buffer[GB_TIMEBUFFER_MAX_LEN] = { 0 };	\
    time::Instance().get_localtime(buffer);

GB_UTILS_NS_BEGIN

	GB_UTILS_CLASS time
	{
	    GB_SINGLETON(time);
	public:
	    void get_localtime(char * const buffer, const unsigned char length = GB_TIMEBUFFER_MAX_LEN)const;
	    std::uint64_t timestamp()const;

	    /*
	     *@brief, format seconds to [?d:][?h:][?m:]?s string
	     */
	    gb::utils::string format(const time_t seconds);
	};

GB_UTILS_NS_END


