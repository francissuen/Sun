#include "concurrency.h"
#include <cassert>
#include <algorithm>
#include <chrono>
#include "time.h"
using gb::utils::concurrency;
typedef concurrency::task_t task_t;
using gb::utils::time;
static void gb::utils::_task_thread(std::uint8_t threadIdx)
{
    concurrency& c = gb::utils::concurrency::Instance();
    std::unique_lock<std::mutex> lck(c._mtx);
    std::condition_variable& cv = c._cv;
    
    std::priority_queue<task_t>& tasks = c._tasks;
    bool& bQuit = c._bQuit;
    bool& bForceQuit = c._bForceQuit;
    for(;;)
    {
	cv.wait(lck, [&]()->bool
		 {
		     return tasks.size() > 0 || bQuit || bForceQuit;
		 }
	    );
	if(!bForceQuit)
	{
	    const size_t taskCount = tasks.size();
	    if(taskCount != 0)
	    {
		task_t task(tasks.top());
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

task_t::task_t(std::function<void(const std::uint8_t,const size_t, void*)> func, void* arg, priority p):
    _bindFunc(std::bind(func, std::placeholders::_1, std::placeholders::_2, arg)),
    _p(p)
{
}

task_t::task_t(const task_t& other):
    _bindFunc(other._bindFunc),
    _p(other._p)
{
}

task_t::task_t(task_t&& other):
    _bindFunc(std::move(other._bindFunc)),
    _p(other._p)
{
}

void task_t::operator=(const task_t& other)
{
    _bindFunc = other._bindFunc;
    _p = other._p;
}
void task_t::operator=(task_t&& other)
{
    _bindFunc = std::move(other._bindFunc);
    _p = other._p;
}


void concurrency::initialize(std::uint8_t threadCount)
{
    std::uint8_t count = threadCount == 0? std::thread::hardware_concurrency() : threadCount;
    assert(count != 0);

    _bQuit = false;
    _bForceQuit = false;

    for(int i = 0; i < count; i++)
    {
	_vThreads.push_back(new std::thread(std::bind(_task_thread, i)));
    }
}

concurrency::~concurrency()
{
    done(true);
}
void concurrency::done(bool bForceQuit)
{
    {
	std::lock_guard<std::mutex> lck(_mtx);
	if(!bForceQuit)
	    _bQuit = true;
	else
	    _bForceQuit = true;
    }

    _cv.notify_all();

    _join();
}

void concurrency::timeout_done(std::function<void(const size_t taskCount)> func, const std::uint32_t timeout)
{
    {
	std::lock_guard<std::mutex> lck(_mtx);
	_bQuit = true;
    }

    _cv.notify_all();

    std::unique_lock<std::mutex> lck(_mtx);
    size_t taskCount = 0;
    for(;;)
    {
	std::cv_status ret = _cv.wait_for(lck, std::chrono::milliseconds(timeout));
	taskCount = _tasks.size();
	lck.unlock();
	if(taskCount == 0)
	{
	    func(taskCount);
	    break;
	}
	
	if(ret == std::cv_status::timeout)
	    func(taskCount);		
	
	lck.lock();
    }

    _join();
}

void concurrency::_join()
{ 
    std::for_each(_vThreads.begin(), _vThreads.end(), [](std::thread*& th)
		  {
		      th->join();
		      delete th;
		  }
	);

    _vThreads.clear();
}

// size_t concurrency::get_taskscount()
// {
//     std::lock_guard<std::mutex> lck(_mtx);
//     return _tasks.size();
// }

void concurrency::pushtask(const task_t& task)
{
    {
	std::lock_guard<std::mutex> lck(_mtx);
	_tasks.push(task);
    }

    _cv.notify_one();
}

void concurrency::pushtask(task_t&& task)
{
    {
	std::lock_guard<std::mutex> lck(_mtx);
	_tasks.push(std::move(task));
    }

    _cv.notify_one();
}
