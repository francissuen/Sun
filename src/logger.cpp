/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#include "logger.h"
#include "config.h"
#include "time.h"
using namespace fs::sun;

/** ref https://en.wikipedia.org/wiki/ANSI_escape_code#Windows_and_DOS */
logger::TermFile::TermFile()
    : color_{
#ifdef _MSC_VER
          FOREGROUND_INTENSITY | FOREGROUND_RED,
          FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
          FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
          FOREGROUND_INTENSITY | FOREGROUND_GREEN,
          FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
          FOREGROUND_INTENSITY
#else
          "\033[91m", "\033[95m", "\033[93m", "\033[92m",
          "\033[96m", "\033[90m"
#endif
      } {
#ifdef _MSC_VER
  console_ = ::GetStdHandle(STD_OUTPUT_HANDLE);
  ::memset(&_preConsoleAttrib, 0, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
  ::GetConsoleScreenBufferInfo(console_, &_preConsoleAttrib);
#endif
}

logger::TermFile::~TermFile() {
  {
#ifdef _MSC_VER
    ::SetConsoleTextAttribute(console_, _preConsoleAttrib.wAttributes);
#else
    std::cout << "\033[0m";
#endif
  }
}

void logger::TermFile::LogRoutine(const std::string& tag,
                                  const std::string& msg,
                                  const Severity s) const {
  {
#ifdef _MSC_VER
    ::SetConsoleTextAttribute(console_, color_[s]);
#else
    std::cout << color_[s];
#endif
    std::cout << Format(tag, msg);
    std::cout.flush();
  }
}

std::string logger::TermFile::Format(const std::string& tag,
                                     const std::string& msg) const {
  return time::LocalTime() + "@tag: " + tag + ", @msg: " + msg + "\n";
}
