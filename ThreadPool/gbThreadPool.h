#pragma once 
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <list>
#include "gbCommon.h"
class gbJob
{
	enum Priority
	{
		low = 1, mid, high
	};
public:
	inline gbJob() {}
	gbJob(std::function<void(void*)> func, void* arg, Priority p = Priority::mid);
	inline bool operator<(const gbJob& other)const { return _p < other._p; }
	inline void Do() { _bindfunc(); }
private:
	std::function<void()> _bindfunc;
	Priority _p;

};
class gbThreadPool
{
	SingletonDeclare(gbThreadPool);

public:
	//if threadCount < 0, then using core's num
	bool Initialize(const int threadCount);
	void PushJob(const gbJob& job);
private:
	static std::list<std::thread*> _lstFreeThreads;
	static std::condition_variable _cv;
	static std::mutex _cv_m;
	static std::priority_queue<gbJob> _jobs;
private:
	static void _infinite_loop();
};