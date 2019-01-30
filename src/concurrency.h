/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <cassert>
#include <vector>
#include <algorithm>
#include "common.h"

#include "config.h"
#include "ns.h"

#define FS_SUN_CONCURRENCY_TASK_PRIORITY_LOW 0x50
#define FS_SUN_CONCURRENCY_TASK_PRIORITY_MID 0x90
#define FS_SUN_CONCURRENCY_TASK_PRIORITY_HIGH 0xd0

FS_SUN_NS_BEGIN

template<typename bindfunc_t>
class _task_base
{
public:
    _task_base(const bindfunc_t func, const std::uint8_t p) :
	_bindFunc(func),
	_p(p)
    {
    }

    _task_base(const _task_base& other) :
	_bindFunc(other._bindFunc),
	_p(other._p)
    {
    }

    _task_base(_task_base&& other) :
	_bindFunc(std::move(other._bindFunc)),
	_p(other._p)
    {
    }

    void operator=(const _task_base& other)
    {
        _bindFunc = other._bindFunc;
        _p = other._p;
    }
    void operator=(_task_base&& other)
    {
        _bindFunc = std::move(other._bindFunc);
        _p = other._p;
    }


    bool operator<(const _task_base& other)const { return _p < other._p; }
protected:
    
    bindfunc_t _bindFunc;       //!< real task func
    std::uint8_t _p;            //!< priority
};

template <typename task_t>
class _concurrency_base
{
protected:
    /**
     * \brief Create a concurrency with a number of threads which have count == \a threadCount.
     * \note If \p threadCount == 0, then the count of hardware cores will be used.
     */
    _concurrency_base(const std::uint8_t threadCount):
        _done(false),
        _doneNormally(true)
    {
        _threadCount = threadCount == 0 ? std::thread::hardware_concurrency() : threadCount;
        FS_SUN_ASSERT(_threadCount != 0);
        
    }
public:
    /**
     * \brief There's no task left need be pushed, you may deallocate all threads allocated before
     * now. Block current thread until all tasks when \p normally is true or running tasks when
     * \p noramally is false have been done.
     */
    void done(bool normally = true)
    {
        {
            std::lock_guard<std::mutex> lck(_mtx);
            _done = true;
            _doneNormally = normally;
        }

        _cv.notify_all();

        _join();
    }

    /**
     * \brief Execute \p func when timeout expires while there.
     * \param timeout in milliseconds
     */
    void timeout_done(std::function<void(const size_t taskCount)> func, const std::uint32_t timeout = 1000)
    {
        {
            std::lock_guard<std::mutex> lck(_mtx);
            _done = true;
        }

        _cv.notify_all();

        std::unique_lock<std::mutex> lck(_mtx);
        size_t taskCount = 0;
        for (;;)
        {
            std::cv_status ret = _cv.wait_for(lck, std::chrono::milliseconds(timeout));
            taskCount = _tasks.size();
            lck.unlock();
            if (taskCount == 0)
            {
                func(taskCount);
                break;
            }

            if (ret == std::cv_status::timeout)
                func(taskCount);

            lck.lock();
        }

        _join();
    }

    void pushtask(const task_t& task)
    {
        {
            std::lock_guard<std::mutex> lck(_mtx);
            _tasks.push(task);
        }

        _cv.notify_one();
    }

    void pushtask(task_t&& task)
    {
        {
            std::lock_guard<std::mutex> lck(_mtx);
            _tasks.push(std::move(task));
        }

        _cv.notify_one();
    }

    std::uint8_t get_threadscount()const { return _threadCount; }

protected:
    std::uint8_t _threadCount;
    std::vector<std::thread> _vThreads;
    bool _done;
    bool _doneNormally;
    std::condition_variable _cv;
    std::mutex _mtx;
    std::priority_queue<task_t> _tasks;

    void _join()
    {
        std::for_each(_vThreads.begin(), _vThreads.end(), [](std::thread& th)
                                                          {
                                                              th.join();
                                                          }
            );

        _vThreads.clear();
    }
};

// template<typename ... otherArgs>
// class task_ti: public _task_base<std::function<void(const std::uint8_t threadIdx)>>
// {
//  public:
//     task_ti(const std::function<void(const std::uint8_t threadIdx, otherArgs ...)>& func,
// 	    otherArgs ... args):
// 	_task_base(std::bind(func, std::placeholders::_1, args ...))
// 	{}
//     void run(const std::uint8_t threadIdx)const
//     {
// 	_bindFunc(threadIdx);
//     }
// };

#define _FS_SUN_CONCURRENCY_TASK_DEFINE(name, task_base_bind_placeholders, bind_func_args, ...) \
    template<typename ... otherArgs>					\
    class name: public _task_base<std::function<void(__VA_ARGS__)>>	\
    {									\
    public:								\
	name(const std::function<void(__VA_ARGS__ FS_SUN__VA_ARGS__COMMA(__VA_ARGS__) otherArgs ...)>& func, \
	     otherArgs ... args, const std::uint8_t p = FS_SUN_CONCURRENCY_TASK_PRIORITY_MID): \
        _task_base(std::bind(func,					\
                             FS_SUN_EXPAND task_base_bind_placeholders	\
                             FS_SUN__VA_ARGS__COMMA task_base_bind_placeholders \
                             args ...), p)				\
	{}								\
	void run(__VA_ARGS__)const					\
	{								\
	    _bindFunc bind_func_args;					\
	}								\
    };    

#define _FS_SUN_UNTILS_CONCURRENCY_DEFINE(name, task_t, task_thread_bind_args, task_run_args, ...) \
    template <typename ... otherArgs>                                   \
    class name: public _concurrency_base<task_t<otherArgs ...>>		\
    {									\
    public:                                                             \
        typedef task_t<otherArgs ...> task;                             \
        name(const std::uint8_t threadCount = 0):                       \
        _concurrency_base<task_t<otherArgs ...>>(threadCount)           \
        {                                                               \
            for(std::uint8_t i = 0; i < this->_threadCount; i++)        \
            {                                                           \
                this->_vThreads.push_back(std::thread(std::bind (_task_thread, \
                                                                 FS_SUN_EXPAND \
                                                                 task_thread_bind_args ))); \
            }                                                           \
        }                                                               \
    private:                                                            \
        static void _task_thread(name* c FS_SUN_COMMA__VA_ARGS__(__VA_ARGS__)) \
        {                                                               \
            std::unique_lock<std::mutex> lck(c->_mtx);                  \
            std::condition_variable& cv = c->_cv;                       \
            std::priority_queue<task_t<otherArgs ...>>& tasks = c->_tasks; \
            const bool & done = c->_done;                               \
            const bool & doneNormally = c->_doneNormally;               \
            for (;;)                                                    \
            {                                                           \
                cv.wait(lck, [&]()->bool                                \
                             {                                          \
                                 return tasks.size() > 0 || done;       \
                             }                                          \
                    );                                                  \
                if (doneNormally)                                       \
                {                                                       \
                    const size_t taskCount = tasks.size();              \
                    if (taskCount != 0)                                 \
                    {                                                   \
                        task_t<otherArgs ...> task(tasks.top());        \
                        tasks.pop();                                    \
                        lck.unlock();                                   \
                        task.run task_run_args;                         \
                        lck.lock();                                     \
                    }                                                   \
                    else                                                \
                    {                                                   \
                        break;                                          \
                    }                                                   \
                }                                                       \
                else                                                    \
                    break;                                              \
            }                                                           \
        }                                                               \
    };


// task_ti(threadIdx, ...)
_FS_SUN_CONCURRENCY_TASK_DEFINE(_task_ti,
                                (std::placeholders::_1),
                                (threadIdx),
                                const std::uint8_t threadIdx)

_FS_SUN_UNTILS_CONCURRENCY_DEFINE(concurrency_ti,
                                  _task_ti,
                                  (this, i),
                                  (threadIdx),
                                  const std::uint8_t threadIdx)

// task_tc(taskCount, ...)
_FS_SUN_CONCURRENCY_TASK_DEFINE(_task_tc,
                                (std::placeholders::_1),
                                (taskCount),
                                const size_t taskCount)

_FS_SUN_UNTILS_CONCURRENCY_DEFINE(concurrency_tc,
                                  _task_tc,
                                  (this),
                                  (taskCount))

// task_ti_tc(threadIdx, taskCount)
_FS_SUN_CONCURRENCY_TASK_DEFINE(_task_ti_tc,
                                (std::placeholders::_1, std::placeholders::_2),
                                (threadIdx, threadCount),
                                const std::uint8_t threadIdx, const size_t threadCount)

_FS_SUN_UNTILS_CONCURRENCY_DEFINE(concurrency_ti_tc,
                                  _task_ti_tc,
                                  (this, i),
                                  (threadIdx, taskCount),
                                  const std::uint8_t threadIdx)

// task(...)
_FS_SUN_CONCURRENCY_TASK_DEFINE(_task, (), ())

_FS_SUN_UNTILS_CONCURRENCY_DEFINE(concurrency,
                                  _task,
                                  (this),
                                  ())

FS_SUN_NS_END
