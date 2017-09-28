#pragma once 
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <list>
#include "gbCommon.h"
class gbTask
{
    enum Priority
    {
	low = 1, mid, high
    };
public:
    inline gbTask() {}
    gbTask(std::function<void(void*)> func, void* arg, Priority p = Priority::mid);
    inline bool operator<(const gbTask& other)const { return _p < other._p; }
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
    void PushTask(const gbTask& job);
    
    void IncreaseThreadBlockingCount();
    void DecreaseThreadBlockingCount(); 
private:
    static std::list<std::thread*> _lstFreeThreads;
    static std::condition_variable _cv;
    static std::mutex _cv_m;
    static std::priority_queue<gbTask> _jobs;
private:
    static void _infinite_loop();

    //TODO, if a task blocking a thread, a new thread should be spawned? And when to destroy this newly spawned thread?
    static unsigned char _threadBlockingCount;
};
