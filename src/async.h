/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_ASYNC_H
#define FS_SUN_ASYNC_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include <mutex>
#include <thread>
#include <tuple>
#include <vector>

#include <iostream>
#include "ns.h"
#include "utility.h"

FS_SUN_NS_BEGIN

template <typename... Signature>
class Async;

template <typename TRet, typename... TArgs>
class Async<TRet(TArgs...)> {
 private:
  struct Package {
    std::promise<TRet> ret;
    std::tuple<typename std::decay<TArgs>::type...> args;
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
  Async(typename std::function<TRet(TArgs...)> func,
        const std::uint8_t threadCount = 1)
      : func_(std::move(func)), quit_(false) {
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
      pkg_buffer_.push_back(Package{
          std::move(promise), std::tuple<typename std::decay<TArgs>::type...>(
                                  std::move(param)...)});
    }
    threadfunc_cv_.notify_one();
    return ret;
  }

  void Finish() {
    std::unique_lock<std::mutex> lck(pkg_buffer_mtx_);
    finished_cv_.wait(lck, [this]() -> bool {
      return pkg_buffer_.size() == 0 &&
             finished_thread_count_.load() == thread_count_;
    });
  }

  std::uint8_t GetThreadCount() const { return thread_count_; }

 private:
  void ThreadFunc() {
    std::unique_lock<std::mutex> lck(pkg_buffer_mtx_);
    for (;;) {
      if (quit_.load()) break;
      if (pkg_buffer_.size() > 0) {
        Package pkg(std::move(pkg_buffer_.front()));
        pkg_buffer_.pop_front();
        lck.unlock();
        Apply2Promise(func_, std::move(pkg.args), pkg.ret);
        lck.lock();
      } else {
        finished_thread_count_.fetch_add(1);
        finished_cv_.notify_all();
        threadfunc_cv_.wait(lck, [this]() -> bool {
          return quit_.load() || pkg_buffer_.size() > 0;
        });
        finished_thread_count_.fetch_add(-1);
      }
    }
  }

 private:
  std::uint8_t thread_count_;
  std::atomic_uint8_t finished_thread_count_{0};
  std::vector<std::thread> threads_;
  std::mutex pkg_buffer_mtx_;
  std::condition_variable threadfunc_cv_;
  std::list<Package> pkg_buffer_;
  std::function<TRet(TArgs...)> func_;
  std::atomic_bool quit_;
  std::condition_variable finished_cv_;
};

template <typename... Signature>
class Concurrent;

template <typename TRet, typename... TArgs>
class Concurrent<TRet(TArgs...)> {
 private:
  struct Package {
    std::promise<TRet> ret;
    std::tuple<typename std::decay<TArgs>::type...> args;
  };

 public:
  Concurrent(typename std::function<TRet(TArgs...)> func)
      : async_(
            [func](Package* begin, Package* end) -> void {
              for (Package* i = begin; i != end; i++) {
                Apply2Promise(func, std::move(i->args), i->ret);
              }
            },
            0) {}

 public:
  std::future<TRet> Push(typename std::decay<TArgs>::type... args) {
    std::promise<TRet> promise;
    std::future<TRet> ret = promise.get_future();
    {
      std::lock_guard<std::mutex> lck(pkg_cache_mtx_);
      pkg_cache_.push_back({std::move(promise), {std::move(args)...}});
    }
    return ret;
  }

  void Run() {
    static const std::uint8_t thread_count = async_.GetThreadCount();
    const std::size_t q = pkg_cache_.size() / thread_count;
    const std::uint8_t r = pkg_cache_.size() % thread_count;
    Package* base = pkg_cache_.data();
    for (std::size_t i = 0; i < thread_count; i++) {
      Package* const begin = base;
      Package* const end = base + q + (i < r ? 1 : 0);
      async_(begin, end);
      base = end;
    }
  }

  void Finish() {
    async_.Finish();
    pkg_cache_.clear();
  }

 private:
  std::vector<Package> pkg_cache_;
  std::mutex pkg_cache_mtx_;
  Async<void(Package*, Package*)> async_;
};

FS_SUN_NS_END

#endif  // FS_SUN_ASYNC_H
