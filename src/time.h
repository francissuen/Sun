/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_TIME_H
#define FS_SUN_TIME_H

#include "singleton.h"
#include "string.h"

FS_SUN_NS_BEGIN

class FS_SUN_API Time : public Singleton<Time> {
  friend class Singleton<Time>;

 private:
  Time() = default;

 public:
  std::string LocalTime() const;
  std::uint64_t Timestamp() const;
};

FS_SUN_NS_END

#endif  // FS_SUN_TIME_H
