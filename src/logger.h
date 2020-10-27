/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_LOGGER_H
#define FS_SUN_LOGGER_H

#include <iostream>
#include "string.h"

#include "async.h"
#include "config.h"
#ifdef _MSC_VER
#include <Windows.h>
#endif

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
    void operator()(std::string msg, const Severity s) {
      async_(default_tag_, std::move(msg), s);
    }
    void operator()(std::string tag, std::string msg, const Severity s) {
      async_(std::move(tag), std::move(msg), s);
    }
    void Flush() { async_.WaitForEmpty(); }

   private:
    file_t file_;
    fs::sun::Async<void(const std::string &, const std::string &,
                        const Severity)>
        async_;
    std::string default_tag_;
  };
};

extern Logger::Log<Logger::TermFile> cout;

#ifdef _MSC_VER
#define FS_SUN_FUNC_NAME __FUNCSIG__
#elif defined(__GNUC__)
#define FS_SUN_FUNC_NAME __PRETTY_FUNCTION__
#endif

#define FS_SUN_LOG(message, severity)                                     \
  {                                                                       \
    std::string msg{fs::sun::string::ToString(message)};                  \
    msg = msg + "\n@FILE: " + __FILE__;                                   \
    msg = msg + ", @LINE: " + fs::sun::string::ToString(__LINE__) + "\n"; \
    msg = msg + "@FUNCTION: " + FS_SUN_FUNC_NAME;                         \
    fs::sun::cout(msg, severity);                                         \
  }

#define FS_SUN_INFO(message) \
  FS_SUN_LOG(message, fs::sun::Logger::Severity::S_INFO)

#define FS_SUN_WARNING(message) \
  FS_SUN_LOG(message, fs::sun::Logger::Severity::S_WARNING)

#define FS_SUN_ERR(message) \
  FS_SUN_LOG(message, fs::sun::Logger::Severity::S_ERROR)

#define FS_SUN_LOG_RETURN(condition, serverity, ...) \
  {                                                  \
    if (condition) {                                 \
      FS_SUN_LOG(#condition, serverity)              \
      return __VA_ARGS__;                            \
    }                                                \
  }

#define FS_SUN_ERR_RET(condition, ...) \
  FS_SUN_LOG_RETURN(condition, fs::sun::Logger::Severity::S_ERROR, __VA_ARGS__)

#define FS_SUN_WARNING_RET(condition, ...)                           \
  FS_SUN_LOG_RETURN(condition, fs::sun::Logger::Severity::S_WARNING, \
                    __VA_ARGS__)

FS_SUN_NS_END

#endif  // FS_SUN_LOGGER_H
