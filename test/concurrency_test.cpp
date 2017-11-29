#include <gbUtils/concurrency.h>
#include <atomic>
#include <cassert>
#include <gbUtils/logger.h>
#include <gbUtils/string.h>
#include <gbUtils/time.h>

using gb::utils::concurrency;
typedef concurrency::task_t task_t;
using gb::utils::logger;
using gb::utils::string;
using gb::utils::time;

int concurrency_test(const unsigned int count = 1000)
{
    const std::uint8_t threadCount = 4;
    concurrency::Instance().initialize(threadCount);
    std::atomic<std::uint8_t> threadSafe_val[threadCount]{{0}};

    unsigned int idx = 0;

    auto task_func = [&idx, &threadSafe_val](const std::uint8_t threadIdx, const size_t taskCount, void* arg)
	{
	    threadSafe_val[threadIdx]++;
	    threadSafe_val[threadIdx]--;
	    assert(threadSafe_val[threadIdx] == 0);
	    idx ++;

	    std::this_thread::sleep_for(std::chrono::milliseconds(3));
	};
    
    
    for(int i = 0; i < count; i++)
    {
	concurrency::Instance().pushtask(
	    task_t(task_func, nullptr));
    }


    const unsigned int timeout = 100;
    auto timeout_func = [&](const size_t taskCount)
	{
	    float value = float(count - taskCount) / count;
	    logger::Instance().progress(value, "hello ");

	};

    concurrency::Instance().timeout_done(timeout_func, timeout);
    logger::Instance().progress_done();

    logger::Instance().log("task_func_2");

    std::mutex mtx;
    auto task_func_2 = [count, &threadSafe_val, &mtx](const std::uint8_t threadIdx, const size_t taskCount, void* arg)
	{
	    threadSafe_val[threadIdx]++;
	    threadSafe_val[threadIdx]--;
	    assert(threadSafe_val[threadIdx] == 0);

	    std::this_thread::sleep_for(std::chrono::milliseconds(30));
	    float value = float(count - (taskCount - 1)) / count;
	    
	    {
		std::lock_guard<std::mutex> lck(mtx);
		logger::Instance().progress(value, string("taskcount:") + (taskCount - 1));	
	    }
	    
	};

    concurrency::Instance().initialize(threadCount);
    for(int i = 0; i < count; i++)
	concurrency::Instance().pushtask(task_t(task_func_2, nullptr));

    concurrency::Instance().done();
    logger::Instance().progress_done();
    
    return idx == count ? 0 : 1;
}
