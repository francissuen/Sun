/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#include "time.h"
#include <ctime>
#include <chrono>
#include <cstring>
#define _FS_SUN_TIMEBUFFER_MAX_LEN_ 128

using namespace fs::Sun;
std::string time::localtime() const
{
    std::string ret(_FS_SUN_TIMEBUFFER_MAX_LEN_,'\0');
    char * buffer = &(ret.front());
    time_t rawTime;
    std::time(&rawTime);
#ifdef _MSC_VER
    ::ctime_s(buffer, _FS_SUN_TIMEBUFFER_MAX_LEN_, &rawTime);
#else
    ::ctime_r(&rawTime, buffer);
#endif
    ret.resize(std::strlen(ret.c_str()));
    return ret;
}

std::uint64_t time::timestamp()const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}
