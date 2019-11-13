/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include <memory>

#include "ns.h"

FS_SUN_NS_BEGIN

template <typename T>
class DeepPtr {
  friend void swap(DeepPtr &a, DeepPtr &b) {
    using std::swap;
    swap(a.ptr_, b.ptr_);
  }

  friend std::string to_string(const DeepPtr &ptr) {
    using std::to_string;
    return to_string(*ptr);
  }

  friend bool operator!=(const DeepPtr &rhs, std::nullptr_t) {
    return rhs.ptr_ != nullptr;
  }

  friend bool operator!=(std::nullptr_t, const DeepPtr &lhs) {
    return nullptr != rhs.ptr_;
  }

 public:
  template <typename... TArgs>
  DeepPtr(TArgs &&... args) : ptr_{new T{std::forward<TArgs>(args)...}} {}

  DeepPtr(const DeepPtr &ptr) : ptr_{ptr != nullptr ? new T(*ptr) : nullptr} {}

  DeepPtr(DeepPtr &&ptr) : ptr_{std::move(ptr.ptr_)} {}

 public:
  DeepPtr &operator=(const DeepPtr &rhs) {
    DeepPtr temp(rhs);
    swap(*this, temp);
  }

  DeepPtr &operator=(DeepPtr &&rhs) { swap(*this, rhs); }

  const T &operator*() const { return *ptr_; }

 private:
  std::unique_ptr<T> ptr_{nullptr};
};

FS_SUN_NS_END
