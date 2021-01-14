/* Copyright (C) 2021 Francis Sun, all rights reserved. */

#ifndef FS_SUN_SMART_VECTOR_H
#define FS_SUN_SMART_VECTOR_H

#include <list>
#include <vector>

#include "ns.h"

FS_SUN_NS_BEGIN

template <typename TElement, typename TAllocator = std::allocator<TElement>>
class SmartVector {
 public:
  std::size_t Push(TElement element) {
    std::size_t idx{0u};
    if (recycled_indices_.size() == 0) {
      idx = container_.size();
      container_.push_back(std::move(element));
    } else {
      idx = recycled_indices_.front();
      assert(idx < container_.size());
      recycled_indices_.pop_front();
      container_[idx] = std::move(element);
    }

    return idx;
  }

  void Pop(const std::size_t idx) {
    assert(idx < container_.size());
    recycled_indices_.push_back(idx);
  }

  std::size_t Size() const {
    return container_.size() - recycled_indices_.size();
  }

  TElement& operator[](const std::size_t idx) { return container_[idx]; }
  const TElement& operator[](const std::size_t idx) const {
    return container_[idx];
  }

 private:
  std::vector<TElement, TAllocator> container_;
  std::list<std::size_t> recycled_indices_;
};

FS_SUN_NS_END

#endif  // FS_SUN_SMART_VECTOR_H
