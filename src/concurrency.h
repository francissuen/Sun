#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <cassert>
#include <vector>
#include <algorithm>
#include "common.h"

#define GB_UTILS_CONCURRENCY_TASK_PRIORITY_LOW 0x50
#define GB_UTILS_CONCURRENCY_TASK_PRIORITY_MID 0x90
#define GB_UTILS_CONCURRENCY_TASK_PRIORITY_HIGH 0xd0
#include "config.h"
#include "ns.h"

GB_UTILS_NS_BEGIN

template<typename bindfunc_t>
class task_base
{
public:
    /*
     *@param func, threadCount is count of current left tasks, including this task
     */
    task_base(const bindfunc_t func, const std::uint8_t p = GB_UTILS_CONCURRENCY_TASK_PRIORITY_MID) :
	_bindFunc(func),
	_p(p)
    {
    }

    task_base(const task_base& other) :
	_bindFunc(other._bindFunc),
	_p(other._p)
    {
    }

    task_base(task_base&& other) :
	_bindFunc(std::move(other._bindFunc)),
	_p(other._p)
    {
    }

    void operator=(const task_base& other)
    {
	_bindFunc = other._bindFunc;
	_p = other._p;
    }
    void operator=(task_base&& other)
    {
	_bindFunc = std::move(other._bindFunc);
	_p = other._p;
    }


    bool operator<(const task_base& other)const { return _p < other._p; }
    //	void run(const std::uint8_t threadIdx, const size_t taskCount)const { _bindFunc(threadIdx, taskCount); }
protected:
    bindfunc_t _bindFunc;
    std::uint8_t _p;
};

template <typename task_t>
class concurrency_base
{
protected:
    /*
     *@param, threadCount, if threadCount == 0, then num of cores is used
     */
    concurrency_base(const std::uint8_t threadCount)
    {
	_threadCount = threadCount == 0 ? std::thread::hardware_concurrency() : threadCount;
	assert(_threadCount != 0);

	_bQuit = false;
	_bForceQuit = false;
    }
public:
    /*
     *@param, bForce, if !bForce, then block current thread until all pushed tasks completed,
     *else block current thread until all current running tasks completed
     */
    void done(bool bForceQuit = false)
    {
	{
	    std::lock_guard<std::mutex> lck(_mtx);
	    if (!bForceQuit)
		_bQuit = true;
	    else
		_bForceQuit = true;
	}

	_cv.notify_all();

	_join();
    }

    /*
     *@brief, execute func when timeout expires until done()
     *@param, timeout, in milliseconds
     */
    void timeout_done(std::function<void(const size_t taskCount)> func, const std::uint32_t timeout = 1000)
    {
	{
	    std::lock_guard<std::mutex> lck(_mtx);
	    _bQuit = true;
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
    bool _bQuit;
    bool _bForceQuit;
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
// class task_ti: public task_base<std::function<void(const std::uint8_t threadIdx)>>
// {
//  public:
//     task_ti(const std::function<void(const std::uint8_t threadIdx, otherArgs ...)>& func,
// 	    otherArgs ... args):
// 	task_base(std::bind(func, std::placeholders::_1, args ...))
// 	{}
//     void run(const std::uint8_t threadIdx)const
//     {
// 	_bindFunc(threadIdx);
//     }
// };

#define _GB_UTILS_CONCURRENCY_TASK_DEFINE(name, task_base_bind_placeholders, bind_func_args, ...) \
    template<typename ... otherArgs>					\
    class name: public task_base<std::function<void(__VA_ARGS__)>>	\
    {									\
    public:								\
	name(const std::function<void(__VA_ARGS__ GB_SMART_COMMA(__VA_ARGS__) otherArgs ...)>& func, \
		otherArgs ... args):					\
	    task_base(std::bind(func,					\
				GB_REMOVE_PARENTHESE task_base_bind_placeholders \
				GB_SMART_COMMA task_base_bind_placeholders \
				args ...))				\
	    {}								\
	void run(__VA_ARGS__)const					\
	{								\
	    _bindFunc bind_func_args;					\
	}								\
    };    

#define _GB_UNTILS_CONCURRENCY_DEFINE(name, task_t, task_thread_bind_args, task_run_args, ...) \
    template <typename ... otherArgs>					\
    class name: public concurrency_base<task_t<otherArgs ...>>		\
    {									\
    public:								\
	name(const std::uint8_t threadCount = 0):			\
	    concurrency_base<task_t<otherArgs ...>>(threadCount)	\
	    {								\
		for(std::uint8_t i = 0; i < this->_threadCount; i++)	\
		    {							\
			this->_vThreads.push_back(std::thread(std::bind (_task_thread, \
									 GB_REMOVE_PARENTHESE \
									 task_thread_bind_args ))); \
		    }							\
	    }								\
    private:								\
	static void _task_thread(name* c, ##__VA_ARGS__) \
	{								\
	    std::unique_lock<std::mutex> lck(c->_mtx);			\
	    std::condition_variable& cv = c->_cv;			\
	    std::priority_queue<task_t<otherArgs ...>>& tasks = c->_tasks; \
	    bool& bQuit = c->_bQuit;					\
	    bool& bForceQuit = c->_bForceQuit;				\
	    for (;;)							\
		{							\
		    cv.wait(lck, [&]()->bool				\
			    {						\
				return tasks.size() > 0 || bQuit || bForceQuit; \
			    }						\
			    );						\
		    if (!bForceQuit)					\
			{						\
			    const size_t taskCount = tasks.size();	\
			    if (taskCount != 0)				\
				{					\
				    task_t<otherArgs ...> task(tasks.top()); \
				    tasks.pop();			\
				    lck.unlock();			\
				    task.run task_run_args;		\
				    lck.lock();				\
				}					\
			    else					\
				{					\
				    assert(bQuit);			\
				    break;				\
				}					\
			}						\
		    else						\
			break;						\
		}							\
	}								\
    };

// task_ti(threadIdx, ...)
_GB_UTILS_CONCURRENCY_TASK_DEFINE(task_ti,
				  (std::placeholders::_1),
				  (threadIdx),
				  const std::uint8_t threadIdx)

_GB_UNTILS_CONCURRENCY_DEFINE(concurrency_ti,
			      task_ti,
			      (this, i),
			      (threadIdx),
			      const std::uint8_t threadIdx)

// task_tc(taskCount, ...)
_GB_UTILS_CONCURRENCY_TASK_DEFINE(task_tc,
				  (std::placeholders::_1),
				  (taskCount),
				  const size_t taskCount)

_GB_UNTILS_CONCURRENCY_DEFINE(concurrency_tc,
			      task_tc,
			      (this),
			      (taskCount))

// task_ti_tc(threadIdx, taskCount)
_GB_UTILS_CONCURRENCY_TASK_DEFINE(task_ti_tc,
				  (std::placeholders::_1, std::placeholders::_2),
				  (threadIdx, threadCount),
				  const std::uint8_t threadIdx, const size_t threadCount)

_GB_UNTILS_CONCURRENCY_DEFINE(concurrency_ti_tc,
			      task_ti_tc,
			      (this, i),
			      (threadIdx, taskCount),
			      const size_t threadIdx)

// task(...)
_GB_UTILS_CONCURRENCY_TASK_DEFINE(task, (), ())

_GB_UNTILS_CONCURRENCY_DEFINE(concurrency,
			      task,
			      (this),
			      ())

GB_UTILS_NS_END
