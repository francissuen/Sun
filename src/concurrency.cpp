#include "concurrency.h"
#include <cassert>
#include <algorithm>
using gb::utils::concurrency;
typedef concurrency::task_t task_t;

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
	    if(tasks.size() != 0)
	    {
		task_t task(tasks.top());
		tasks.pop();

		lck.unlock();

		task.run(threadIdx);

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

task_t::task_t(std::function<void(std::uint8_t, void*)> func, void* arg, priority p):
    _bindFunc(std::bind(func, std::placeholders::_1, arg)),
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

    _cv.notify_one();

    std::for_each(_vThreads.begin(), _vThreads.end(), [](std::thread*& th)
		  {
		      th->join();
		      delete th;
		  }
	);

    _vThreads.clear();
}
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
