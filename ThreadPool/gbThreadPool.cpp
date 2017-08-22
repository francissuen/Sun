#include "gbThreadPool.h"
#include "Log/gbLog.h"
#include "String/gbString.h"

gbTask::gbTask(std::function<void(void*)> func, void* arg, Priority p):
	_p(p),
	_bindfunc(std::bind(func, arg))
{
}

std::list<std::thread*> gbThreadPool::_lstFreeThreads;
std::condition_variable gbThreadPool::_cv;
std::mutex gbThreadPool::_cv_m;
std::priority_queue<gbTask> gbThreadPool::_jobs;

bool gbThreadPool::Initialize(const int threadCount)
{
	int count = threadCount < 0 ? std::thread::hardware_concurrency() : threadCount;
	gbLog::Instance().Log(gbString("threadCount: ") + count);
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
	while (true)
	{
		std::unique_lock<std::mutex> m(_cv_m);
		if (_jobs.size() > 0)
		{
			gbLog::Instance().Log("i am woken up now @"); std::cout << std::this_thread::get_id() << std::endl;

			gbTask job;
			{
				job = _jobs.top();
				_jobs.pop();
			}
			m.unlock();

			job.Do();
		}
		else
			_cv.wait(m);
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