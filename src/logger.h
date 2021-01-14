/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_LOGGER_H
#define FS_SUN_LOGGER_H

#include <iostream>

#ifdef _MSC_VER
#include <Windows.h>
#endif

#include "async.h"
#include "config.h"
#include "string.h"

FS_SUN_NS_BEGIN

class FS_SUN_API Logger {
 public:
  enum Severity : std::uint8_t {
    S_FATAL = 0,
    S_ERROR,
    S_WARNING,
    S_INFO,
    S_DEBUG,
    S_VERBOSE,
    S_MAX
  };

  /** terminal file */
  class FS_SUN_API TermFile {
   public:
    TermFile();
    ~TermFile();

   public:
    void LogRoutine(const std::string &tag, const std::string &msg,
                    const Severity s) const;

   private:
    std::string Format(const std::string &tag, const std::string &msg) const;

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
  class Log {
   public:
    Log(const char *default_tag)
        : async_{std::bind(&file_t::LogRoutine, &file_, std::placeholders::_1,
                           std::placeholders::_2, std::placeholders::_3)},
          default_tag_{default_tag} {}

    ~Log() { Flush(); }

   public:
    void operator()(std::string msg, const Severity s = Severity::S_INFO) {
      async_(default_tag_, std::move(msg), s);
    }
    void operator()(std::string tag, std::string msg, const Severity s) {
      async_(std::move(tag), std::move(msg), s);
    }
    void Flush() { async_.Finish(); }

   private:
    file_t file_;
    fs::sun::Async<void(const std::string &, const std::string &,
                        const Severity)>
        async_;
    std::string default_tag_;
  };
};

extern Logger::Log<Logger::TermFile> cout;

#define FS_SUN_INFO(message) \
  { fs::sun::cout(message, fs::sun::Logger::Severity::S_INFO); }

#define FS_SUN_WARN(message) \
  { fs::sun::cout(message, fs::sun::Logger::Severity::S_WARNING); }

#define FS_SUN_ERROR(message) \
  { fs::sun::cout(message, fs::sun::Logger::Severity::S_ERROR); }

#ifdef FS_SUN_NO_NS
using namespace fs::sun;
#define INFO(message) FS_SUN_INFO(message)
#define WARN(message) FS_SUN_WARN(message)
#define ERROR(message) FS_SUN_ERROR(message)
#endif  // FS_SUN_NO_NS

FS_SUN_NS_END

#endif  // FS_SUN_LOGGER_H
