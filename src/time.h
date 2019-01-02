#pragma once
#include <stdio.h>
#include <ctime>
#include "common.h"
#include "config.h"
#include "string.h"
#define FS_TIMEBUFFER_MAX_LEN 64

#define FS_GET_LOCALTIME(buffer)		\
    char buffer[FS_TIMEBUFFER_MAX_LEN] = {0};	\
    time::Instance().get_localtime(buffer);

FS_COMMON_NS_BEGIN

	FS_COMMON_CLASS time
	{
	    FS_SINGLETON(time);
	public:
	    void get_localtime(char * const buffer, const unsigned char length = FS_TIMEBUFFER_MAX_LEN)const;
	    std::uint64_t timestamp()const;

	    /*
	     *@brief, format seconds to [?d:][?h:][?m:]?s string
	     */
	    fs::common::string format(const time_t seconds);
	};

FS_COMMON_NS_END


