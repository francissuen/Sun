#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <vector>
#include "common.h"
namespace gb
{
    namespace utils
    {
	static void _task_thread(std::uint8_t);
	class concurrency
	{
	public:
	    class task_t
	    {
	    public:
		enum priority
		{
		    low = 1,
		    mid, high
		};
		task_t(std::function<void(std::uint8_t, void*)> func, void* arg, priority p = priority::mid);
		task_t(const task_t& other);
		task_t(task_t&& other);
		void operator=(const task_t& other);
		void operator=(task_t&& other);
		inline bool operator<(const task_t& other)const{ return _p < other._p; }
		inline void run(std::uint8_t threadIdx)const{ _bindFunc(threadIdx); }
	    private:
		std::function<void(std::uint8_t)> _bindFunc;
		priority _p;
	    };

	    struct status_t
	    {
		inline status_t():
		    taskCount(0),
		    speed(0)
		    {}
		size_t taskCount;//count of task left
		size_t speed;//in tasks/seconds
		time_t eta;//time left in seconds
	    };
	    GB_SINGLETON(concurrency);
	    ~concurrency();
	public:
	    /*
	     *@param, threadCount, if threadCount == 0, then num of cores is used
	     */
	    void initialize(std::uint8_t threadCount = 0);

	    /*
	     *@param, bForce, if !bForce, then block current thread until all pushed tasks completed,
	     *else block current thread until all current running tasks completed
	     */
	    void done(bool bForce = false);

	    /*
	     *@brief, execute func when timeout expires until done()
	     *@param, timeout, in milliseconds
	     */
	    void timeout_done(std::function<void(const status_t& status)> func, const std::uint32_t timeout = 1000);

	    void pushtask(const task_t& task);
	    void pushtask(task_t&& task);

	    inline std::uint8_t get_threadscount()const { return _vThreads.size(); }
	    size_t get_taskscount();
	private:
	    std::vector<std::thread*> _vThreads;
	    bool _bQuit;
	    bool _bForceQuit;
	    std::condition_variable _cv;
	    std::mutex _mtx;
	    std::priority_queue<task_t> _tasks;

	    size_t _preTaskCount;
	    
	    friend void _task_thread(std::uint8_t);
	    void _join();
	};
    };
};

