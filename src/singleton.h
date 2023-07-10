/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_SINGLETON_H
#define FS_SUN_SINGLETON_H

#include "ns.h"

#define FS_SUN_SINGLETON(class_name)       \
 public:                                   \
  inline static class_name &Instance() {   \
    static class_name instance;            \
    return instance;                       \
  }                                        \
                                           \
 private:                                  \
  class_name(const class_name &) = delete; \
  class_name &operator=(const class_name &) = delete;

FS_SUN_NS_BEGIN

template <typename T>
class Singleton {
 public:
  static T &Instance() {
    static T instance;
    return instance;
  }
  Singleton(const Singleton &) = delete;
  Singleton &operator=(const Singleton &) = delete;

 protected:
  Singleton() = default;
  ~Singleton() = default;
};

template <typename T>
T &GetSingleton() {
  static T instance;
  return instance;
}

FS_SUN_NS_END

#endif  // FS_SUN_SINGLETON_H
