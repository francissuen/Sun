#include "../src/async.h"
#include "../src/string.h"

using namespace fs::Sun;

int async_test()
{
    async<void, const string &> a([](const string & str) {}, 1);
    const string str("hello");
    const auto ret = a(str);
    ret.wait();
    return 0;
}
