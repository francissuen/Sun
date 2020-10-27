/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_ASYNC_H
#define FS_SUN_ASYNC_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>

#include "ns.h"
#include "utility.h"

FS_SUN_NS_BEGIN

template <typename... Ts>
class Async;

template <typename TRet, typename... TArgs>
class Async<TRet(TArgs...)> {
 private:
  struct Package {
    std::promise<TRet> ret;
    std::tuple<typename std::decay<TArgs>::type...> params;
  };

  template <typename T>
  struct TArgsValidator {
    static constexpr bool value =
        std::is_reference<T>::value
            ? std::is_const<typename std::remove_reference<T>::type>::value
            : true;
  };

  static_assert(
      std::conditional<sizeof...(TArgs) == 0, std::true_type,
                       StaticAnd<TArgsValidator, TArgs...>>::type::value,
      "TArgs only accepts const reference or non-reference type");

 public:
  Async(const typename std::function<TRet(TArgs...)>& func,
        const std::uint8_t threadCount = 1)
      : func_(func), quit_(false) {
    thread_count_ =
        threadCount == 0 ? std::thread::hardware_concurrency() : threadCount;
    threads_.reserve(thread_count_);

    for (std::uint8_t i = 0; i < thread_count_; i++) {
      threads_.push_back(std::thread(std::bind(&Async::ThreadFunc, this)));
    }
  }

  ~Async() {
    quit_.store(true);
    threadfunc_cv_.notify_all();
    for (auto& t : threads_) {
      t.join();
    }
  }

 public:
  std::future<TRet> operator()(typename std::decay<TArgs>::type... param) {
    std::promise<TRet> promise;
    std::future<TRet> ret = promise.get_future();
    {
      std::lock_guard<std::mutex> lck(pkg_buffer_mtx_);
      pkg_buffer_.push(Package{std::move(promise),
                               std::tuple<typename std::decay<TArgs>::type...>(
                                   std::move(param)...)});
    }
    threadfunc_cv_.notify_one();
    return ret;
  }

  void WaitForEmpty() {
    std::unique_lock<std::mutex> lck(pkg_buffer_mtx_);
    if (pkg_buffer_.size() > 0) {
      empty_cv_.wait(lck,
                     [this]() -> bool { return pkg_buffer_.size() == 0; });
    }
  }

 private:
  void ThreadFunc() {
    std::unique_lock<std::mutex> lck(pkg_buffer_mtx_);
    for (;;) {
      if (quit_.load()) break;
      if (pkg_buffer_.size() > 0) {
        Package pkg(std::move(pkg_buffer_.front()));
        pkg_buffer_.pop();
        lck.unlock();
        Apply2Promise(func_, std::move(pkg.params), pkg.ret);
        lck.lock();
      } else {
        empty_cv_.notify_all();
        threadfunc_cv_.wait(lck);
      }
    }
  }

 private:
  std::uint8_t thread_count_;
  std::vector<std::thread> threads_;
  std::mutex pkg_buffer_mtx_;
  std::condition_variable threadfunc_cv_;
  std::queue<Package> pkg_buffer_;
  std::function<TRet(TArgs...)> func_;
  std::atomic_bool quit_;
  std::condition_variable empty_cv_;
};

FS_SUN_NS_END
#endif  // FS_SUN_ASYNC_H
