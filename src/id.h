/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_ID_H
#define FS_SUN_ID_H

#include <cstddef>
#include <memory>
#include <unordered_map>

#include "ns.h"

FS_SUN_NS_BEGIN

template <typename T, typename TID = std::size_t>
class IDVault {
 public:
  using ID = TID;

 public:
  static constexpr ID INVALID_ID{0};

 public:
  ID InsertData(const std::shared_ptr<T>& data) {
    id_++;
    const ID id = id_;
    data_.insert(std::make_pair(id, data));
    return id;
  }

  void EraseData(const ID id) { data_.erase(id); }

  std::shared_ptr<T> GetData(const ID id) {
    const auto& itr = data_.find(id);
    if (itr != data_.end())
      return itr->second;
    else
      return nullptr;
  }

  const std::shared_ptr<T> GetData(const ID id) const {
    return const_cast<IDVault*>(this)->GetData(id);
  }

  const std::unordered_map<ID, std::shared_ptr<T>>& GetDatas() const {
    return data_;
  }

 private:
  ID id_{0};
  std::unordered_map<ID, std::shared_ptr<T>> data_;
};

FS_SUN_NS_END

#endif  // FS_SUN_ID_H
