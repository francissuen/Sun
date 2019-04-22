#include "../src/async.h"
#include "../src/string.h"

using namespace fs::Sun;

int async_test()
{
    async<const string &> a([](const string & str) {}, 1);
    const string str("hello");
    a(str);
    a.done();
    return 0;
}