/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#pragma once
#include <functional>
#include <memory>
#include <unordered_map>

#include "debug.h"
#include "logger.h"
#include "ns.h"

FS_SUN_NS_BEGIN

using FactoryOrderNumber = std::size_t;

template <typename TBase, typename... TCtorArgs>
class Factory {
 public:
  using OrderNumber = FactoryOrderNumber;
  using GoodsBase = TBase;
  using Ctor = std::function<std::unique_ptr<GoodsBase>(TCtorArgs...)>;

  template <typename... TGoods>
  class With {
   public:
    operator Factory<GoodsBase, TCtorArgs...>() {
      Factory<GoodsBase, TCtorArgs...> f;
      f.template Construct<TGoods...>();
      return f;
    }

    template <typename T>
    static constexpr OrderNumber OrderNumberOf() {
      return IndexOf<T>::template In<TGoods...>::value;
    }
  };

 private:
  template <typename T, typename... TGoods>
  struct ConstructOne {
    operator std::pair<const OrderNumber, Ctor>() {
      /** non-odr-used version */
      static constexpr OrderNumber order_num =
          IndexOf<T>::template In<TGoods...>::value;
      return {order_num, [](TCtorArgs... args) -> std::unique_ptr<GoodsBase> {
                return std::unique_ptr<GoodsBase>(new T(args...));
              }};
    }
  };

 public:
  std::unique_ptr<GoodsBase> Create(const OrderNumber order_num,
                                    TCtorArgs... args) const {
    const auto& itr = ctors_.find(order_num);
    if (itr != ctors_.end())
      return itr->second(args...);
    else {
      cout("No corresponding ctor found @order_num: " +
               std::to_string(order_num),
           Logger::S_ERROR);
      return nullptr;
    }
  }

 private:
  template <typename... TGoods>
  void Construct() {
    ctors_ = std::unordered_map<OrderNumber, Ctor>{
        ConstructOne<TGoods, TGoods...>()...};
  }

 private:
  std::unordered_map<OrderNumber, Ctor> ctors_;
};

FS_SUN_NS_END
