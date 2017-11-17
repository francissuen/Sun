#include "time.h"
#include <cassert>

using gb::utils::time;

void time::get_localtime(char * const buffer, const unsigned char length)const
{
    assert(buffer != nullptr);
    time_t rawTime;
    ::time(&rawTime);

    tm* pTime = localtime(&rawTime);
    assert(pTime != nullptr);
    
    sprintf(buffer, "%d:%d:%d(%d,%d,%d)",
	    pTime->tm_hour,
	    pTime->tm_min,
	    pTime->tm_sec,
	    (pTime->tm_mon + 1),
	    pTime->tm_mday,
	    1900+pTime->tm_year);
}

time_t time::get_localtime()const
{
    time_t rawTime;
    ::time(&rawTime);
    return rawTime;
}
