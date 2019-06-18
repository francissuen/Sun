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

FS_SUN_CLASS Logger
{
public:
enum Severity : std::uint8_t
{
    S_FATAL = 0,
    S_ERROR,
    S_WARNING,
    S_INFO,
    S_DEBUG,
    S_VERBOSE,
    S_MAX
};

FS_SUN_CLASS TermFile          /** terminal file */
{
public:
TermFile();
~TermFile();
public:
void LogRoutine(const std::string & tag, const std::string & msg, const Severity s) const;
private:
std::string Format(const std::string & tag, const std::string & msg) const;
private:
#ifdef _MSC_VER
const WORD color_[Severity::S_MAX];
HANDLE console_;
CONSOLE_SCREEN_BUFFER_INFO _preConsoleAttrib;
#else
const std::string color_[Severity::S_MAX];
#endif
};

template <typename file_t>
class Log
{
public:
    Log(const char* default_tag):
        async_{std::bind(&file_t::LogRoutine, &file_,
                         std::placeholders::_1,
                         std::placeholders::_2,
                         std::placeholders::_3)},
        default_tag_{default_tag}
    {
    }
    
public:
    void operator()(std::string msg, const Severity s)
    {
        async_(default_tag_, std::move(msg), s);
    }
    void operator()(std::string tag, std::string msg, const Severity s)
    {
        async_(std::move(tag), std::move(msg), s);
    }
    void Flush()
    {
        async_.WaitForEmpty();
    }
private:
    /** void LogRoutine(const std::string & tag, const std::string & msg, const Severity s) */
    /** { */
    /**     file_.Log(tag, msg, s); */
    /** } */
private:
    file_t file_;
    fs::sun::Async<void(const std::string & , const std::string &, const Severity)> async_;
    std::string default_tag_;
};
};

extern Logger::Log<Logger::TermFile> cout;

#define FS_SUN_DEBUG_LOG        /**TODO */

FS_SUN_NS_END
