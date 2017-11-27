#include <gbUtils/concurrency.h>
#include <atomic>
#include <cassert>
#include <gbUtils/logger.h>
#include <gbUtils/string.h>
#include <gbUtils/time.h>
#include <numeric>
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

    auto task_func = [&idx, &threadSafe_val](const std::uint8_t threadIdx, void* arg)
	{
	    threadSafe_val[threadIdx]++;
	    threadSafe_val[threadIdx]--;
	    assert(threadSafe_val[threadIdx] == 0);
	    idx ++;

	    std::this_thread::sleep_for(std::chrono::milliseconds(30));
	};
    
    
    for(int i = 0; i < count; i++)
    {
	concurrency::Instance().pushtask(
	    task_t(task_func, nullptr));
	    // task_t([i, &idx, &threadSafe_val](const std::uint8_t threadIdx, void* arg)
	    // 					{
	    // 					    threadSafe_val[threadIdx]++;
	    // 					    threadSafe_val[threadIdx]--;
	    // 					    assert(threadSafe_val[threadIdx] == 0);
	    // 					    idx ++;
	    // 					}, nullptr));
    }

    const unsigned int timeout = 10;
    auto timeout_func = [&](const concurrency::status_t status)
	{
	    unsigned int speed = status.speed;
	    unsigned int timeLeft = status.eta;
	    if(speed != 0)
		logger::Instance().log(string("task left: ") + status.taskCount + "\n"
				   "eta: " + time::Instance().format(timeLeft) + "\n"
				   "speed@" + speed + " tasks/s"
		);
	};
    concurrency::Instance().timeout_done(timeout_func, timeout);

    return idx == count ? 0 : 1;
}
