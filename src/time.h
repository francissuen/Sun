/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include <stdio.h>
#include <ctime>
#include "common.h"
#include "config.h"
#include "string.h"
#define FS_SUN_TIMEBUFFER_MAX_LEN 64

#define FS_SUN_GET_LOCALTIME(buffer)		\
    char buffer[FS_SUN_TIMEBUFFER_MAX_LEN] = {0};	\
    time::Instance().get_localtime(buffer);

FS_SUN_NS_BEGIN

	FS_SUN_CLASS time
	{
	    FS_SUN_SINGLETON(time);
	public:
	    void get_localtime(char * const buffer, const unsigned char length = FS_SUN_TIMEBUFFER_MAX_LEN)const;
	    std::uint64_t timestamp()const;

	    /*
	     *@brief, format seconds to [?d:][?h:][?m:]?s string
	     */
	    fs::Sun::string format(const time_t seconds);
	};

FS_SUN_NS_END


