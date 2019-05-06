/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include <iostream>
#include <string>
#include "config.h"
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
void log(const std::string & tag, const std::string & msg, const severity s) const;
private:
std::string _format(const std::string & tag, const std::string & msg) const;
private:
#ifdef _MSC_VER
const WORD _color[severity::S_MAX];
HANDLE _hConsole;
CONSOLE_SCREEN_BUFFER_INFO _preConsoleAttrib;
#else
const std::string _color[severity::S_MAX];
#endif
};

template <typename file_t>
class log
{
public:
    log(const char* default_tag):
        _async(std::bind(&log::_log, this,
                         std::placeholders::_1,
                         std::placeholders::_2,
                         std::placeholders::_3)),
        _default_tag(default_tag)
    {}
    
public:
    void operator()(std::string msg, const severity s)
    {
        _async(_default_tag, std::move(msg), s);
    }
    void operator()(std::string tag, std::string msg, const severity s)
    {
        _async(std::move(tag), std::move(msg), s);
    }
    void flush()
    {
        _async.wait_for_empty();
    }
private:
    void _log(const std::string & tag, const std::string & msg, const severity s)
    {
        _file.log(tag, msg, s);
    }
private:
    file_t _file;
    async<void, const std::string & , const std::string &, const severity> _async;
    std::string _default_tag;
};
};

extern logger::log<logger::term_file> cout;

#define FS_SUN_DEBUG_LOG        /**TODO */

FS_SUN_NS_END
