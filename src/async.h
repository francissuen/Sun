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
#include "common.h"

FS_SUN_NS_BEGIN

template <typename ... param_t>
class async
{
public:
    async(const typename std::function<void(param_t ...)> & func,
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
        for(auto & t : _threads)
        {
            t.join();
        }
    }

public:
    /** TODO result */
    void operator()(param_t ... param)
    {
        {
            std::lock_guard<std::mutex> lck(_paramsMtx);
            _params.push(std::tuple<param_t ...>(param ...));
        }
        _cv.notify_one();
    }

    void done()
    {
        std::unique_lock<std::mutex> lck(_paramsMtx);
        if(_params.size() > 0)
        {
            _doneCV.wait(lck, [this]() -> bool
                              {
                                  return _params.size() == 0;
                              });
        }
    }

private:
    void _thread_func()
    {
        std::unique_lock<std::mutex> lck(_paramsMtx);
        for(;;)
        {
            _cv.wait(lck, [this]() -> bool
                     {
                         return _params.size() > 0;
                     });

            std::tuple<param_t ...> param(_params.back());
            _params.pop();
            lck.unlock();
            /** int a = std::tuple_size<std::tuple<const fs::Sun::string &>>::value; */
            apply(_func, param);
            lck.lock();
            if(_params.size() == 0)
                _doneCV.notify_all();
            if(_quit.load())
                break;
        }
    }
private:
    std::uint8_t _threadCount;
    std::vector<std::thread> _threads;
    std::mutex _paramsMtx;
    std::condition_variable _cv;
    std::queue<std::tuple<param_t ...>> _params;
    const std::function<void(param_t ...)> _func;
    std::atomic_bool _quit;
    std::condition_variable _doneCV;
};

FS_SUN_NS_END
