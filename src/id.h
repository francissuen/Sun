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
  inline T Get() { return id_++; };

 private:
  std::atomic<T> id_{T{0}};
};
}  // namespace generator
}  // namespace id

template <typename T, typename TGenerator = id::generator::Increment<T>>
class ID {
 public:
  using IDType = T;

  template <typename S>
  class Of {
   public:
    using IDType = T;
    using TID = ID;

   private:
    static TGenerator generator_;

   public:
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
template <typename S>
G ID<T, G>::Of<S>::generator_;

template <typename T>
using ID32 = ID<std::uint32_t>::Of<T>;

template <typename TType, typename TID>
class IDVault {
 public:
  using ID = TID;
  using Container = std::unordered_map<typename ID::IDType, TType>;
  using Iterator = typename Container::iterator;
  using ConstIterator = typename Container::const_iterator;

 public:
  // iterators
  Iterator begin() { return container_.begin(); }
  ConstIterator begin() const { return container_.begin(); }

  Iterator end() { return container_.end(); }
  ConstIterator end() const { return container_.end(); }

 public:
  typename ID::IDType Insert(const typename ID::IDType id, TType data) {
    const auto ret = container_.insert(std::make_pair(id, std::move(data)));
    if (ret.second)
      return ret.first->first;
    else
      return ID::TID::INVALID_ID;
  }

  typename ID::IDType Push(TType data) {
    return PushData(ID{}, std::move(data));
  }

  bool HasID(const typename ID::IDType id) const {
    return container_.find(id) != container_.cend();
  }

  TType& GetData(const typename ID::IDType id) {
    auto itr = container_.find(id);
    if (itr != container_.end()) return itr->second;
  }

  const std::unordered_map<typename ID::IDType, TType>& GetDatas() const {
    return container_;
  }

 private:
  Container container_;
};

template <typename TType>
using IDVault32 = IDVault<TType, ID32<TType>>;

FS_SUN_NS_END

#endif  // FS_SUN_ID_H
