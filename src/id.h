/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_ID_H
#define FS_SUN_ID_H

#include <cstddef>
#include <list>

#include "ns.h"

FS_SUN_NS_BEGIN

template <typename T, typename TID = std::size_t>
class ID {
 public:
  static TID Get() {
    static TID id{0};
    id++;
    return id;
  }
};

FS_SUN_NS_END

#endif  // FS_SUN_ID_H
