/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_FACTORY_H
#define FS_SUN_FACTORY_H

#include <functional>
#include <memory>
#include <unordered_map>

#include "logger.h"
#include "ns.h"

FS_SUN_NS_BEGIN

template <typename TKey, typename... TCtorArgs>
class Factory {
 public:
  using Ctor = std::function<void*(TCtorArgs...)>;

 public:
  template <typename T>
  bool Register(TKey key) {
    return ctors_
        .insert(std::make_pair(std::move(key),
                               [](TCtorArgs... args) -> void* {
                                 return new T(
                                     std::forward<TCtorArgs>(args)...);
                               }))
        .second;
  }

  void Unregister(const TKey& key) {
    const auto& itr = ctors_.find(key);
    if (itr != ctors_.end())
      ctors_.erase(itr);
    else
      FS_SUN_WARN("No ctor found for key: " + string::ToString(key))
  }

  template <typename TRet>
  std::unique_ptr<TRet> Create(const TKey& key, TCtorArgs... args) const {
    const auto& itr = ctors_.find(key);
    if (itr != ctors_.end())
      return std::unique_ptr<TRet>(reinterpret_cast<TRet*>(
          itr->second(std::forward<TCtorArgs>(args)...)));
    else {
      FS_SUN_ERROR("No ctor found for key: " + string::ToString(key))
      return nullptr;
    }
  }

 private:
  std::unordered_map<TKey, Ctor> ctors_;
};

FS_SUN_NS_END

#endif  // FS_SUN_FACTORY_H
