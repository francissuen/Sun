#include "../src/concurrency.h"
#include <atomic>
#include <cassert>
#include "../src/logger.h"
#include "../src/string.h"
#include "../src/time.h"

using namespace gb::utils;

int concurrency_test(const unsigned int count = 1000)
{
    const std::uint8_t threadCount = 4;
    
    concurrency_ti<> c_ti(threadCount);
    
    std::atomic<std::uint8_t> threadSafe_val[threadCount]{{0}};

    unsigned int idx = 0;

    std::function<void(const std::uint8_t)> task_func = [&idx, &threadSafe_val](const std::uint8_t threadIdx)
	{
	    threadSafe_val[threadIdx]++;
	    threadSafe_val[threadIdx]--;
	    assert(threadSafe_val[threadIdx] == 0);
	    idx ++;

	    std::this_thread::sleep_for(std::chrono::milliseconds(3));
	};
    
    for(unsigned int i = 0; i < count; i++)
    {
	c_ti.pushtask(task_ti<>(task_func
#ifdef _MSC_VER
				, GB_UTILS_CONCURRENCY_TASK_PRIORITY_MID
#endif
			  ));
    }

    const unsigned int timeout = 100;
    auto timeout_func = [&](const size_t taskCount)
	{
	    float value = float(count - taskCount) / count;
	    logger::Instance().progress(value, "hello ");

	};

    c_ti.timeout_done(timeout_func, timeout);
    logger::Instance().progress_done();

    logger::Instance().log("task_func_2");

    std::mutex mtx;
    std::function<void(const std::uint8_t, const size_t, int)> task_func_2 = [count, &threadSafe_val, &mtx](const std::uint8_t threadIdx, const size_t taskCount, int arg)
	{
	    threadSafe_val[threadIdx]++;
	    threadSafe_val[threadIdx]--;
	    assert(threadSafe_val[threadIdx] == 0);

	    std::this_thread::sleep_for(std::chrono::milliseconds(30));
	    float value = float(count - (taskCount - 1)) / count;
	    
	    {
		std::lock_guard<std::mutex> lck(mtx);
		logger::Instance().progress(value, string("arg:") + arg);	
	    }
	    
	};

    concurrency_ti_tc<int> c_ti_tc(threadCount);
    for(unsigned int i = 0; i < count; i++)
	c_ti_tc.pushtask(task_ti_tc<int>(task_func_2, i
#ifdef _MSC_VER
					 , GB_UTILS_CONCURRENCY_TASK_PRIORITY_MID
#endif
						  ));

    c_ti_tc.done();
    logger::Instance().progress_done();
    
    return idx == count ? 0 : 1;
}
