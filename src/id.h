/* Copyright (C) 2021 Francis Sun, all rights reserved. */

#ifndef FS_SUN_ID_H
#define FS_SUN_ID_H

#include <atomic>
#include <cinttypes>
#include <limits>
#include <unordered_map>

#include "ns.h"

FS_SUN_NS_BEGIN

template <typename TID>
class IDIncrementalGenerator {
 public:
  constexpr static TID IVALID_ID{std::numeric_limits<TID>::max()};

 public:
  TID Get() { return id_.fetch_add(1, std::memory_order_relaxed); }

 private:
  std::atomic<TID> id_;
};

template <typename TObject, typename TID = std::size_t,
          typename TIDGenerator = IDIncrementalGenerator<TID>>
class IDObject {
 public:
  using IDType = TID;

 public:
  constexpr static TID INVALID_ID{TIDGenerator::INVALID_ID};

 private:
  static TIDGenerator id_generator_;

 public:
  IDObject() : id_{id_generator_.Get()} {}

 public:
  TID GetID() const { return id_; }

 protected:
  const TID id_;
};

template <typename TObject, typename TID, typename TIDGnerator>
constexpr TID IDObject<TObject, TID, TIDGnerator>::INVALID_ID;

template <typename TObject, typename TID, typename TIDGnerator>
TIDGnerator IDObject<TObject, TID, TIDGnerator>::id_generator_;

FS_SUN_NS_END

#endif  // FS_SUN_ID_H
