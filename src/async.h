/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_ASYNC_H
#define FS_SUN_ASYNC_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "ns.h"
#include "utility.h"

FS_SUN_NS_BEGIN

struct ThreadIndex {
  static constexpr const std::uint8_t npos{
      std::numeric_limits<std::uint8_t>::max()};
  std::uint8_t idx{0u};
};

template <typename... Signature>
class Async;

template <typename... Signature>
class AsyncBatched;

template <typename TRet, typename... TArgs>
class Async<TRet(TArgs...)> {
  friend class AsyncBatched<TRet(TArgs...)>;

 private:
  struct Package {
    using Args = std::tuple<typename std::decay<TArgs>::type...>;
    std::promise<TRet> ret;
    Args args;
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

  struct HasThreadIndex {
    static constexpr const bool value =
        IsType<ThreadIndex>::In<typename std::decay<TArgs>::type...>::value;
  };
  static_assert((!HasThreadIndex::value) ||
                    (HasThreadIndex::value &&
                     IndexOf<ThreadIndex>::In<
                         typename std::decay<TArgs>::type...>::value == 0),
                "If there has the type ThreadIndex in TArgs, then the index "
                "of it must be 0");

  template <bool has_thread_idx, typename TDummy = void>
  struct HelperImpl;

  template <typename TDummy>
  struct HelperImpl<false, TDummy> {
    template <typename... TCArgs>
    static typename Package::Args ConstructArgs(TCArgs... args) {
      return {std::move(args)...};
    }
    static void SetThreadIdx(const std::uint8_t thread_idx,
                             typename Package::Args& args) {}
  };

  template <typename TDummy>
  struct HelperImpl<true, TDummy> {
    template <typename... TCArgs>
    static typename Package::Args ConstructArgs(TCArgs... args) {
      return {{ThreadIndex::npos}, std::move(args)...};
    }
    static void SetThreadIdx(const std::uint8_t thread_idx,
                             typename Package::Args& args) {
      std::get<0>(args) = {thread_idx};
    }
  };

  using Helper = HelperImpl<HasThreadIndex::value>;

 public:
  Async() {}
  Async(typename std::function<TRet(TArgs...)> func,
        const std::uint8_t thread_count = 1) {
    SetFunction(std::move(func), thread_count);
  }

 public:
  ~Async() { Quit(); }
  Async(const Async&) = delete;
  Async& operator=(const Async&) = delete;

 public:
  void SetFunction(typename std::function<TRet(TArgs...)> func,
                   std::uint8_t thread_count = 1) {
    Quit();

    quit_ = false;
    func_ = std::move(func);
    if (thread_count == 0) thread_count = std::thread::hardware_concurrency();
    threads_.reserve(thread_count);

    for (std::uint8_t i = 0; i < thread_count; i++) {
      threads_.push_back(std::thread(std::bind(&Async::ThreadFunc, this, i)));
    }
  }

  template <typename... TUserArgs>
  std::future<TRet> operator()(TUserArgs... args) {
    std::promise<TRet> promise;
    std::future<TRet> ret = promise.get_future();
    {
      std::unique_lock<std::mutex> blocking_lck{blocking_mtx_};
      blocking_cv_.wait(blocking_lck, [this]() { return !is_blocking_; });
      blocking_lck.unlock();

      std::lock_guard<std::mutex> lck(pkg_buffer_mtx_);
      pkg_buffer_.push_back(Package{
          std::move(promise), Helper::ConstructArgs(std::move(args)...)});
    }
    threadfunc_cv_.notify_one();
    return ret;
  }

  void Block() {
    {
      std::lock_guard<std::mutex> lck{blocking_mtx_};
      is_blocking_ = true;
    }
  }

  void Unblock() {
    {
      std::lock_guard<std::mutex> lck{blocking_mtx_};
      is_blocking_ = false;
      blocking_cv_.notify_all();
    }
  }

  void Quit() {
    {
      std::lock_guard<std::mutex> lck{pkg_buffer_mtx_};
      quit_ = true;
      threadfunc_cv_.notify_all();
    }
    for (auto& t : threads_) {
      t.join();
    }
    threads_.clear();
  }

  void Finish() {
    {
      std::unique_lock<std::mutex> lck(pkg_buffer_mtx_);
      likely_finished_cv_.wait(lck, [this]() -> bool {
        std::lock_guard<std::mutex> lck{finished_thread_count_mtx};
        return pkg_buffer_.size() == 0 &&
               finished_thread_count_ == threads_.size();
      });
    }
  }

  std::uint8_t GetThreadCount() const { return threads_.size(); }

 private:
  void ThreadFunc(const std::uint8_t thread_idx) {
    std::unique_lock<std::mutex> lck(pkg_buffer_mtx_);
    for (;;) {
      if (quit_) break;
      if (pkg_buffer_.size() > 0) {
        Package pkg(std::move(pkg_buffer_.front()));
        pkg_buffer_.pop_front();
        lck.unlock();
        Helper::SetThreadIdx(thread_idx, pkg.args);
        Apply2Promise(func_, std::move(pkg.args), pkg.ret);
        lck.lock();
      } else {
        {
          std::lock_guard<std::mutex> lck{finished_thread_count_mtx};
          finished_thread_count_ += 1u;
          likely_finished_cv_.notify_all();
        }
        threadfunc_cv_.wait(lck, [this]() -> bool {
          return quit_ || pkg_buffer_.size() > 0u;
        });
        {
          std::lock_guard<std::mutex> lck{finished_thread_count_mtx};
          finished_thread_count_ -= 1u;
        }
      }
    }
  }

 private:
  std::uint8_t finished_thread_count_{0u};
  std::mutex finished_thread_count_mtx;
  std::vector<std::thread> threads_;
  std::mutex pkg_buffer_mtx_;
  std::condition_variable threadfunc_cv_;
  std::list<Package> pkg_buffer_;
  std::function<TRet(TArgs...)> func_;
  bool quit_{false};
  std::condition_variable likely_finished_cv_;
  bool is_blocking_{false};
  std::mutex blocking_mtx_;
  std::condition_variable blocking_cv_;
};

/**
 *                  add
 *                 \ | /
 *                 commit
 *                 / | \
 *              run concurrently
 */
template <typename TRet, typename... TArgs>
class AsyncBatched<TRet(TArgs...)> {
 private:
  using ThisAsync = Async<TRet(TArgs...)>;
  using Package = typename ThisAsync::Package;
  using Helper = typename ThisAsync::Helper;

 public:
  AsyncBatched(typename std::function<TRet(TArgs...)> func)
      : commiter_{std::bind(&AsyncBatched::Commiter, this,
                            std::placeholders::_1)},
        worker_{[func](const ThreadIndex thread_idx, Package* begin,
                       Package* end) -> void {
                  for (Package* i = begin; i != end; i++) {
                    Helper::SetThreadIdx(thread_idx.idx, i->args);
                    Apply2Promise(func, std::move(i->args), i->ret);
                  }
                },
                0} {}

 public:
  template <typename... TUserArgs>
  std::future<TRet> Add(TUserArgs... args) {
    std::promise<TRet> promise;
    std::future<TRet> ret = promise.get_future();
    {
      std::lock_guard<std::mutex> lck(staging_pkg_mtx_);
      staging_pkg_.push_back(
          {std::move(promise), Helper::ConstructArgs(std::move(args)...)});
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
  Async<void(const ThreadIndex, Package*, Package*)> worker_;
};

FS_SUN_NS_END

#endif  // FS_SUN_ASYNC_H
