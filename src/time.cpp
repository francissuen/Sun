/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#include "time.h"

#include <chrono>
#include <cstring>
#include <ctime>
#define FS_SUN_TIME_BUFFER_MAX_LEN 128

using namespace fs::sun;
std::string Time::LocalTime() const {
  std::string ret(FS_SUN_TIME_BUFFER_MAX_LEN, '\0');
  char* buffer = &(ret.front());
  time_t rawTime;
  std::time(&rawTime);
#ifdef _MSC_VER
  ::ctime_s(buffer, FS_SUN_TIME_BUFFER_MAX_LEN, &rawTime);
#else
  ::ctime_r(&rawTime, buffer);
#endif
  ret.resize(std::strlen(ret.c_str()));
  return ret;
}

std::uint64_t Time::Timestamp() const {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}
