/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#include "logger.h"
#include "time.h"
using namespace fs::Sun;

#define _FS_SUN_LOGGER_DEFAULT_TAG_ "Sun"

logger::log<logger::term_file> fs::Sun::cout(_FS_SUN_LOGGER_DEFAULT_TAG_);

/** ref https://en.wikipedia.org/wiki/ANSI_escape_code#Windows_and_DOS */
logger::term_file::term_file():
#ifdef _MSC_VER
    _color{FOREGROUND_INTENSITY | FOREGROUND_RED,
           FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
           FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
           FOREGROUND_INTENSITY | FOREGROUND_GREEN,
           FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
           FOREGROUND_INTENSITY}
#else
    _color{"\033[91m",
           "\033[95m",
           "\033[93m",
           "\033[92m",
           "\033[96m",
           "\033[90m"}
#endif
{
#ifdef _MSC_VER
    _hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
    ::memset(&_preConsoleAttrib, 0, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    ::GetConsoleScreenBufferInfo(_hConsole, &_preConsoleAttrib);
#endif
}

logger::term_file::~term_file()
{
#ifdef _MSC_VER
    ::SetConsoleTextAttribute(_hConsole, _preConsoleAttrib.wAttributes);
#else
    std::cout << "\033[0m";
#endif
}

void logger::term_file::log(const std::string & tag, const std::string & msg, const severity s) const
{
    #ifdef _MSC_VER
    ::SetConsoleTextAttribute(_hConsole, _color[s]);
    #else
    std::cout << _color[s];
    #endif
    std::cout << _format(tag, msg);
    std::cout.flush();
}

std::string logger::term_file::_format(const std::string & tag, const std::string & msg) const
{
    return time::instance().localtime() + "@tag: " + tag + ", @msg: " + msg + "\n";
}
