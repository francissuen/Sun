#include <gbUtils/concurrency.h>

using gb::utils::concurrency;
typedef concurrency::task_t task_t;

int concurrency_test(const unsigned int count = 100)
{
    const std::uint8_t threadCount = 4;
    concurrency::Instance().initialize(threadCount);
    std::uint8_t threadSafe_val[threadCount] = {0};

    unsigned int idx = 0;
    for(int i = 0; i < count; i++)
    {
	concurrency::Instance().pushtask(task_t([i, &idx, &threadSafe_val](const std::uint8_t threadIdx, void* arg)
						{
						    threadSafe_val[threadIdx]++;
						    threadSafe_val[threadIdx]--;
						    assert(threadSafe_val[threadIdx] == 0);
						    idx ++;
						}, nullptr));
    }

    concurrency::Instance().done();

    return idx == count ? 0 : 1;
}
