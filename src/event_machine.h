/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_EVENT_MACHINE_H
#define FS_SUN_EVENT_MACHINE_H

#include <functional>
#include <list>
#include <memory>
#include <unordered_map>

#include "ns.h"

FS_SUN_NS_BEGIN

template <typename TEvent, typename TTargetID, typename... TFuncArgs>
class EventMachine {
 public:
  using Handler = std::function<void(const TEvent, TFuncArgs... args)>;

 public:
  void Register(const TEvent ev, const TTargetID target_id, Handler handler) {
    const auto &itr = handlers_.find(ev);
    if (itr != handlers_.end()) {
      itr->second.insert(std::make_pair(target_id, std::move(handler)));
    } else
      handlers_.insert(
          std::make_pair(ev, std::unordered_map<TTargetID, Handler>{
                                 {target_id, std::move(handler)}}));
  }

  void Unregister(const TEvent ev, const TTargetID target_id) {
    const auto &itr = handlers_.find(ev);
    if (itr != handlers_.end()) {
      auto &handlers = itr->second;
      const auto &handler_itr = handlers.find(target_id);
      if (handler_itr != handlers.end()) handlers.erase(handler_itr);
    }
  }

  template <typename... TArgs>
  void DispatchEvent(const TEvent &ev, TArgs &&... args) {
    const auto &itr = handlers_.find(ev);
    if (itr != handlers_.end()) {
      for (const auto &handler : itr->second) {
        handler.second(ev, std::forward<TArgs>(args)...);
      }
    }
  }

 private:
  std::unordered_map<TEvent, std::unordered_map<TTargetID, Handler>> handlers_;
};

FS_SUN_NS_END

#endif  // FS_SUN_EVENT_MACHINE_H
