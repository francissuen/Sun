/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#pragma once

#include <functional>
#include <list>
#include <memory>
#include <unordered_map>

#include "ns.h"

FS_SUN_NS_BEGIN

template <typename TEvent, typename THandlerID = std::uintptr_t>
class EventMachine {
 public:
  using Handler =
      std::function<void(const TEvent &, const std::shared_ptr<void> &)>;
  struct EventData {
    TEvent ev;
    std::shared_ptr<void> user_ptr;
  };

 public:
  void Register(const TEvent &ev, const THandlerID &handler_id,
                const Handler &handler) {
    const auto &itr = handlers_.find(ev);
    if (itr != handlers_.end()) {
      itr->second.insert(std::make_pair(handler_id, handler));
    } else
      handlers_.insert(std::make_pair(ev, {handler_id, handler}));
  }

  void Unregister(const TEvent &ev, const THandlerID &handler_id) {
    const auto &itr = handlers_.find(ev);
    if (itr != handlers_.end()) {
      auto &handlers = itr->second;
      const auto &handler_itr = handlers.find(handler_id);
      if (handler_itr != handlers.end()) handlers.erase(handler_itr);
    }
  }

  void PostEvent(const TEvent &ev) { event_data_queue_.push_back(ev); }

  void DispatchEvent(const TEvent &ev) {
    const auto &itr = handlers_.find(ev);
    if (itr != handlers_.end()) {
      for (const auto &handler : itr->second) {
        handler->second(ev);
      }
    }
  }

  bool GetEvent(EventData &ev) {
    if (event_data_queue_.size() > 0) {
      ev = std::move(event_data_queue_.front());
      event_data_queue_.pop_front();
      return true;
    } else
      return false;
  }

 private:
  std::unordered_map<TEvent, std::unordered_map<THandlerID, Handler>>
      handlers_;
  std::list<EventData> event_data_queue_;
};

FS_SUN_NS_END
