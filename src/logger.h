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

namespace logger {
enum Severity : std::uint8_t {
  S_FATAL = 0,
  S_ERROR,
  S_WARNING,
  S_INFO,
  S_DEBUG,
  S_VERBOSE,
  S_COUNT
};

/** terminal file */
class FS_SUN_API TermFile {
 private:
  static std::mutex mtx_;

 public:
  TermFile();

 public:
  ~TermFile();
  TermFile(const TermFile &) = delete;
  TermFile &operator=(const TermFile &) = delete;

 public:
  void LogRoutine(const std::string &tag, const std::string &msg,
                  const logger::Severity s) const;

 private:
  std::string Format(const std::string &tag, const std::string &msg) const;

 private:
#ifdef _MSC_VER
  const WORD color_[logger::Severity::S_COUNT];
  HANDLE console_;
  CONSOLE_SCREEN_BUFFER_INFO _preConsoleAttrib;
#else
  const std::string color_[logger::Severity::S_COUNT];
#endif
};
}  // namespace logger

template <typename file_t = logger::TermFile>
class Logger {
 public:
  static Logger &Instance() {
    static Logger instance{FS_SUN_NAME};
    return instance;
  }

 public:
  Logger(const char *default_tag)
      : async_log_{std::bind(&file_t::LogRoutine, &file_,
                             std::placeholders::_1, std::placeholders::_2,
                             std::placeholders::_3)},
        default_tag_{default_tag} {}

 public:
  ~Logger() { Flush(); }
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

 public:
  void Log(std::string msg,
           const logger::Severity s = logger::Severity::S_INFO) {
    async_log_(default_tag_, std::move(msg), s);
  }
  void Log(std::string tag, std::string msg, const logger::Severity s) {
    async_log_(std::move(tag), std::move(msg), s);
  }
  void Flush() { async_log_.Finish(); }

 private:
  file_t file_;
  fs::sun::Async<void(const std::string &, const std::string &,
                      const logger::Severity)>
      async_log_;
  std::string default_tag_;
};

#ifdef NDEBUG
#define FS_SUN_LOGGER_DEBUG_MSG(...) (__VA_ARGS__)
#else
#define FS_SUN_LOGGER_DEBUG_MSG(...)              \
  (std::string(__VA_ARGS__) + "\n" __FILE__ ":" + \
   fs::sun::string::ToString(__LINE__))
#endif

#define FS_SUN_INFO(...)                                                    \
  {                                                                         \
    fs::sun::Logger<>::Instance().Log(FS_SUN_LOGGER_DEBUG_MSG(__VA_ARGS__), \
                                      fs::sun::logger::Severity::S_INFO);   \
  }

#define FS_SUN_WARN(...)                                                     \
  {                                                                          \
    fs::sun::Logger<>::Instance().Log(FS_SUN_LOGGER_DEBUG_MSG(__VA_ARGS__),  \
                                      fs::sun::logger::Severity::S_WARNING); \
  }

#define FS_SUN_ERROR(...)                                                   \
  {                                                                         \
    fs::sun::Logger<>::Instance().Log(FS_SUN_LOGGER_DEBUG_MSG(__VA_ARGS__), \
                                      fs::sun::logger::Severity::S_ERROR);  \
  }

#ifdef FS_SUN_NO_NS
using namespace fs::sun;
#define INFO(message) FS_SUN_INFO(message)
#define WARN(message) FS_SUN_WARN(message)
#define ERROR(message) FS_SUN_ERROR(message)
#endif  // FS_SUN_NO_NS

FS_SUN_NS_END

#endif  // FS_SUN_LOGGER_H
