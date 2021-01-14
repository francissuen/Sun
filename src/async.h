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
#include <unordered_map>
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
  Async() {}
  Async(typename std::function<TRet(TArgs...)> func,
        const std::uint8_t thread_count = 1) {
    SetFunction(std::move(func), thread_count);
  }

  ~Async() { Quit(); }

 public:
  void SetFunction(typename std::function<TRet(TArgs...)> func,
                   std::uint8_t thread_count = 1) {
    Quit();

    quit_.store(false);
    func_ = std::move(func);
    if (thread_count == 0) thread_count = std::thread::hardware_concurrency();
    threads_.reserve(thread_count);

    for (std::uint8_t i = 0; i < thread_count; i++) {
      threads_.push_back(std::thread(std::bind(&Async::ThreadFunc, this, i)));
    }
  }

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

  void Quit() {
    quit_.store(true);
    threadfunc_cv_.notify_all();
    for (auto& t : threads_) {
      t.join();
    }
    threads_.clear();
  }

  void Finish() {
    std::unique_lock<std::mutex> lck(pkg_buffer_mtx_);
    finished_cv_.wait(lck, [this]() -> bool {
      return pkg_buffer_.size() == 0 &&
             finished_thread_count_.load() == threads_.size();
    });
  }

  std::uint8_t GetThreadCount() const { return threads_.size(); }

  std::uint8_t GetThreadIdx() const {
    const auto& idx = thread_indices_.find(std::this_thread::get_id());
    if (idx != thread_indices_.end())
      return idx->second;
    else
      return GetThreadCount();
  }

 private:
  void ThreadFunc(const std::uint8_t thread_idx) {
    {
      std::lock_guard<std::mutex> lck(thread_indices_mtx_);
      thread_indices_.insert(
          std::make_pair(std::this_thread::get_id(), thread_idx));
    }
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
  std::unordered_map<std::thread::id, std::uint8_t> thread_indices_;
  std::mutex thread_indices_mtx_;
  std::atomic_uint8_t finished_thread_count_{0u};
  std::vector<std::thread> threads_;
  std::mutex pkg_buffer_mtx_;
  std::condition_variable threadfunc_cv_;
  std::list<Package> pkg_buffer_;
  std::function<TRet(TArgs...)> func_;
  std::atomic_bool quit_{false};
  std::condition_variable finished_cv_;
};

/**
 *                  add
 *                 \ | /
 *                 commit
 *                 / | \
 *              run concurrently
 */
template <typename... Signature>
class AsyncBatched;

template <typename TRet, typename... TArgs>
class AsyncBatched<TRet(TArgs...)> {
 private:
  struct Package {
    std::promise<TRet> ret;
    std::tuple<typename std::decay<TArgs>::type...> args;
  };

 public:
  AsyncBatched(typename std::function<TRet(TArgs...)> func)
      : commiter_{std::bind(&AsyncBatched::Commiter, this,
                            std::placeholders::_1)},
        worker_{[func](Package* begin, Package* end) -> void {
                  for (Package* i = begin; i != end; i++) {
                    Apply2Promise(func, std::move(i->args), i->ret);
                  }
                },
                0} {}

 public:
  std::future<TRet> Add(typename std::decay<TArgs>::type... args) {
    std::promise<TRet> promise;
    std::future<TRet> ret = promise.get_future();
    {
      std::lock_guard<std::mutex> lck(staging_pkg_mtx_);
      staging_pkg_.push_back({std::move(promise), {std::move(args)...}});
    }
    return ret;
  }

  void Commit() {
    {
      std::lock_guard<std::mutex> lck(staging_pkg_mtx_);
      commiter_(std::move(staging_pkg_));
    }
  }

  void Finish() { commiter_.Finish(); }

  std::uint8_t GetThreadCount() const { return worker_.GetThreadCount(); }

 private:
  void Commiter(std::vector<Package> pkg_cache) {
    static const std::uint8_t thread_count = worker_.GetThreadCount();
    const std::size_t q = pkg_cache.size() / thread_count;
    const std::uint8_t r = pkg_cache.size() % thread_count;
    Package* base = pkg_cache.data();
    for (std::size_t i = 0; i < thread_count; i++) {
      Package* const begin = base;
      Package* const end = base + q + (i < r ? 1 : 0);
      worker_(begin, end);
      base = end;
    }
    worker_.Finish();
  }

 private:
  std::vector<Package> staging_pkg_;
  std::mutex staging_pkg_mtx_;
  Async<void(std::vector<Package>)> commiter_;
  Async<void(Package*, Package*)> worker_;
};

FS_SUN_NS_END

#endif  // FS_SUN_ASYNC_H
