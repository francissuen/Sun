/* Copyright (C) 2021 Francis Sun, all rights reserved. */

#ifndef FS_SUN_ID_H
#define FS_SUN_ID_H

#include <atomic>
#include <cinttypes>
#include <limits>
#include <unordered_map>

#include "ns.h"

FS_SUN_NS_BEGIN

namespace id {
namespace generator {
template <typename T>
class Increment {
 public:
  static constexpr T INVALID_ID{std::numeric_limits<T>::max()};

 public:
  inline T Get() {
    return id_.fetch_add(1, std::memory_order::memory_order_relaxed);
  };

 private:
  std::atomic<T> id_{T{0}};
};
}  // namespace generator
}  // namespace id

template <typename TID, typename TGenerator = id::generator::Increment<TID>>
class ID {
 public:
  using IDType = TID;

  template <typename TObject>
  class Of {
   private:
    static TGenerator generator_;

   protected:
    Of() : id_{generator_.Get()} {}

   public:
    inline operator IDType() const { return GetID(); }

   public:
    inline IDType GetID() const { return id_; }

   protected:
    const IDType id_;
  };

 public:
  static constexpr IDType INVALID_ID{TGenerator::INVALID_ID};
};

template <typename T, typename G>
template <typename O>
G ID<T, G>::Of<O>::generator_;

template <typename T>
using ID32 = ID<std::uint32_t>::Of<T>;

FS_SUN_NS_END

#endif  // FS_SUN_ID_H
