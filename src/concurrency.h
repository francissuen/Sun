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

template<typename ... TaskArg>
static void _task_thread(std::uint8_t threadIdx);

template<typename ... TaskArg>
class concurrency
{
public:
    class task_t
    {
    public:
	/*
	 *@param func, threadCount is count of current left tasks, including this task
	 */
	task_t(const std::function<void(const std::uint8_t, const size_t, TaskArg ...)>& func, TaskArg ... arg, const std::uint8_t p = GB_UTILS_CONCURRENCY_TASK_PRIORITY_MID) :
	    _bindFunc(std::bind(func, std::placeholders::_1, std::placeholders::_2, arg ...)),
	    _p(p)
	    {
	    }

	task_t(const task_t& other) :
	    _bindFunc(other._bindFunc),
	    _p(other._p)
	    {
	    }

	task_t(task_t&& other) :
	    _bindFunc(std::move(other._bindFunc)),
	    _p(other._p)
	    {
	    }

	void operator=(const task_t& other)
	    {
		_bindFunc = other._bindFunc;
		_p = other._p;
	    }
	void operator=(task_t&& other)
	    {
		_bindFunc = std::move(other._bindFunc);
		_p = other._p;
	    }


	bool operator<(const task_t& other)const { return _p < other._p; }
	void run(const std::uint8_t threadIdx, const size_t taskCount)const { _bindFunc(threadIdx, taskCount); }
    private:
	std::function<void(const std::uint8_t, const size_t)> _bindFunc;
	std::uint8_t _p;
    };


    GB_SINGLETON(concurrency);
    ~concurrency()
	{
	    done(true);
	}
public:
    /*
     *@param, threadCount, if threadCount == 0, then num of cores is used
     */
    void initialize(std::uint8_t threadCount = 0)
	{
	    std::uint8_t count = threadCount == 0 ? std::thread::hardware_concurrency() : threadCount;
	    assert(count != 0);

	    _bQuit = false;
	    _bForceQuit = false;

	    for (int i = 0; i < count; i++)
	    {
		_vThreads.push_back(std::thread(std::bind(_task_thread<TaskArg ...>, i)));
	    }
	}

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

    std::uint8_t get_threadscount()const { return _vThreads.size(); }

    //retrive count in taskfunc, or timeoutfunc
//	    size_t get_taskscount();
private:
    std::vector<std::thread> _vThreads;
    bool _bQuit;
    bool _bForceQuit;
    std::condition_variable _cv;
    std::mutex _mtx;
    std::priority_queue<task_t> _tasks;

    size_t _preTaskCount;
    friend void _task_thread<TaskArg ...>(std::uint8_t threadIdx);
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

template<typename ... TaskArg>
static void _task_thread(std::uint8_t threadIdx)
{
    concurrency<TaskArg ...>& c = concurrency<TaskArg ...>::Instance();
    std::unique_lock<std::mutex> lck(c._mtx);
    std::condition_variable& cv = c._cv;

    std::priority_queue<typename concurrency<TaskArg ...>::task_t>& tasks = c._tasks;
    bool& bQuit = c._bQuit;
    bool& bForceQuit = c._bForceQuit;
    for (;;)
    {
	cv.wait(lck, [&]()->bool
		{
		    return tasks.size() > 0 || bQuit || bForceQuit;
		}
	    );
	if (!bForceQuit)
	{
	    const size_t taskCount = tasks.size();
	    if (taskCount != 0)
	    {
		typename concurrency<TaskArg ...>::task_t task(tasks.top());
		tasks.pop();

		lck.unlock();

		task.run(threadIdx, taskCount);

		lck.lock();
	    }
	    else
	    {
		assert(bQuit);
		break;
	    }
	}
	else
	    break;
    }
}

GB_UTILS_NS_END
