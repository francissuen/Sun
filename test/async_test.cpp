#include "../src/async.h"
#include "../src/string.h"

using namespace fs::Sun;

int async_test()
{
    std::function<void(const string &)> func = [](const string & str)
                                       {
                                           string s = str + "a";
                                       };
    async<const string &> a(func, 1);
    a(string("hello"));

    a.done();
    return 0;
}
