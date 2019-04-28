/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#include "time.h"
#include <ctime>
#include <chrono>

#define _FS_SUN_TIMEBUFFER_MAX_LEN_ 128

using namespace fs::Sun;
string time::localtime() const
{
    string ret(_FS_SUN_TIMEBUFFER_MAX_LEN_,'\0');
    char * buffer = &(ret.front());
    time_t rawTime;
    std::time(&rawTime);
    tm retTime = {};
#ifdef _MSC_VER
    tm* pTime = nullptr;
    if(! ::localtime_s(&retTime, &rawTime))
        pTime = &retTime;
#else
    tm* pTime = ::localtime_r(&rawTime, &ret);
#endif
    assert(pTime != nullptr);
    
    sprintf(buffer, "%d:%d:%d[%d/%d/%d]",
	    pTime->tm_hour,
	    pTime->tm_min,
	    pTime->tm_sec,
	    (pTime->tm_mon + 1),
	    pTime->tm_mday,
	    1900+pTime->tm_year);
    ret.resize(std::strlen(ret.data()));
    return ret;
}

std::uint64_t time::timestamp()const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

string time::format(const time_t seconds)
{
    const time_t min = seconds / 60;
    const std::uint8_t s = seconds % 60;
    if(min > 0)
    {
	const time_t hour = min / 60;
	const std::uint8_t m = min % 60;
	
	if(hour > 0)
	{
	    time_t day = hour / 24;
	    const std::uint8_t h = hour % 24;
	    if(day > 0)
	    {
		if(day > 999)
		    day = 999;
		return (unsigned int)day + string("d") + ":" +
		    (unsigned int)h + string("h") + ":" +
		    (unsigned int)m + "m" + ":" +
		    (unsigned int)s + "s";
	    }
	    else
		return h + string("h") + ":" +
		    (unsigned int)m + "m" + ":" +
		    (unsigned int)s + "s";
	}
	else
	    return (unsigned int)m + string("m") + ":" + (unsigned int)s + "s";
    }
    else
	return (unsigned int)s + string("s");
}
