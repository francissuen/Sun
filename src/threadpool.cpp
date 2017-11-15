#include "threadpool.h"
#include "logger.h"
#include "string.h"

using gb::utils::string;
using gb::utils::logger;

gbTask::gbTask(std::function<void(void*)> func, void* arg, Priority p):
    _p(p),
    _bindfunc(std::bind(func, arg))
{
}

std::list<std::thread*> gbThreadPool::_lstFreeThreads;
std::condition_variable gbThreadPool::_cv;
std::mutex gbThreadPool::_cv_m;
std::priority_queue<gbTask> gbThreadPool::_jobs;
unsigned char gbThreadPool::_threadBlockingCount = 0;

bool gbThreadPool::Initialize(const int threadCount)
{
    int count = threadCount < 0 ? std::thread::hardware_concurrency() : threadCount;
    logger::Instance().log(string("threadCount: ") + count);
    for (int i = 0; i < count; i++)
    {
	std::thread* t = new std::thread(_infinite_loop);
	if (t == nullptr)
	    return false;
    }

    return true;
}

void gbThreadPool::_infinite_loop()
{
    std::unique_lock<std::mutex> m(_cv_m);
    for(;;)
    {
	_cv.wait(m, [&]()->bool
		 {
		     return _jobs.size() > 0;
		 });

	    gbTask job;
	    job = _jobs.top();
	    m.unlock();
	    job.Do();

	    m.lock();
	    _jobs.pop();
    }
}

void gbThreadPool::PushTask(const gbTask& job)
{

    {
	std::lock_guard<std::mutex> lg(_cv_m);
	_jobs.push(job);
    }

    _cv.notify_one();
}

// void gbThreadPool::IncreaseThreadActiveCount()
// {
//     _threadActiveCount++;
// }
// void gbThreadPool::DecreaseThreadActiveCount()
// {
//     _threadActiveCount--;
// }
