/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include <iostream>
#include "string.h"
#include "async.h"

#ifdef _MSC_VER
#include<Windows.h>
#endif

FS_SUN_NS_BEGIN

FS_SUN_CLASS logger
{
public:
enum severity : std::uint8_t
{
    S_FATAL = 0,
    S_ERROR,
    S_WARNING,
    S_INFO,
    S_DEBUG,
    S_VERBOSE,
    S_MAX
};

FS_SUN_CLASS term_file          /** terminal file */
{
public:
term_file();
~term_file();
public:
void log(const string & tag, const string & msg, const severity s) const;
private:
string _format(const string & tag, const string & msg) const;
private:
#ifdef _MSC_VER
const WORD _color[severity::S_MAX];
HANDLE _hConsole;
CONSOLE_SCREEN_BUFFER_INFO _preConsoleAttrib;
#else
const string _color[severity::S_MAX];
#endif
};

template <typename file_t>
class log
{
public:
    log():
        _async(std::bind(&log::_log, this,
                         std::placeholders::_1,
                         std::placeholders::_2,
                         std::placeholders::_3))
    {}
    
public:
    void operator()(string tag, string msg, const severity s)
    {
        _async(tag, msg, s);
    }
private:
    void _log(const string & tag, const string & msg, const severity s)
    {
        _file.log(tag, msg, s);
    }
private:
    file_t _file;
    async<void, const string & , const string &, const severity> _async;
};
};

extern logger::log<logger::term_file> cout;

FS_SUN_NS_END
