/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include <thread>
#include <functional>
#include <queue>
#include <tuple>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>

#include "common.h"

FS_SUN_NS_BEGIN

template <typename ret_t, typename ... param_t>
class async
{
private:
    struct _package_
    {
        std::promise<ret_t> ret;
        std::tuple<typename std::decay<param_t>::type ...> params;
    };
    
public:
    async(const typename std::function<ret_t(param_t ...)> & func,
          const std::uint8_t threadCount = 1):
        _func(func),
        _quit(false)
    {
        _threadCount = threadCount == 0 ?
        std::thread::hardware_concurrency():
        threadCount;
        _threads.reserve(_threadCount);

        for(std::uint8_t i = 0; i < _threadCount; i++)
        {
            _threads.push_back(std::thread(std::bind(&async::_thread_func, this)));
        }
    }

    ~async()
    {
        _quit.store(true);
        _threadFuncCV.notify_all();
        for(auto & t : _threads)
        {
            t.join();
        }
    }

public:
    std::future<ret_t> operator()(typename std::decay<param_t>::type ... param)
    {
        std::promise<ret_t> promise;
        std::future<ret_t> ret = promise.get_future();
        {
            std::lock_guard<std::mutex> lck(_pkgBufferMtx);
            _pkgBuffer.push(_package_{std::move(promise),
                                      std::tuple<
                                      typename std::decay<param_t>::type ...>(
                                          std::move(param) ...)});
        }
        _threadFuncCV.notify_one();
        return ret;
    }

    void wait_for_empty()
    {
        std::unique_lock<std::mutex> lck(_pkgBufferMtx);
        if(_pkgBuffer.size() > 0)
        {
            _emptyCV.wait(lck, [this]() -> bool
                              {
                                  return _pkgBuffer.size() == 0;
                              });
        }
    }

private:
    void _thread_func()
    {
        std::unique_lock<std::mutex> lck(_pkgBufferMtx);
        for(;;)
        {
            if(_quit.load())
                break;
            
            if(_pkgBuffer.size() > 0)
            {
                _package_ pkg(std::move(_pkgBuffer.back()));
                _pkgBuffer.pop();
                lck.unlock();
                apply2promise(_func, std::move(pkg.params), pkg.ret);
            }
            else
            {
                _emptyCV.notify_all();
                lck.lock();
                _threadFuncCV.wait(lck);                
            }
        }
    }
    
private:
    std::uint8_t _threadCount;
    std::vector<std::thread> _threads;
    std::mutex _pkgBufferMtx;
    std::condition_variable _threadFuncCV;
    std::queue<_package_> _pkgBuffer;
    const std::function<void(param_t ...)> _func;
    std::atomic_bool _quit;
    std::condition_variable _emptyCV;
};

FS_SUN_NS_END
