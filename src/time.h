/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_TIME_H
#define FS_SUN_TIME_H

#include "singleton.h"
#include "string.h"

FS_SUN_NS_BEGIN

namespace time {
std::string LocalTime();
std::uint64_t Timestamp();
}  // namespace time

FS_SUN_NS_END

#endif  // FS_SUN_TIME_H
