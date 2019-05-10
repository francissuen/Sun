#include "../src/logger.h"
#include "../src/variant.h"
#include "../src/string.h"
using namespace fs::Sun;

struct stTest
{
    stTest(const int _a):
        a(_a)
    {
        cout("stTest called @a: " + std::to_string(a), logger::S_INFO);
    }
    ~stTest()
    {
        cout("~stTest called.", logger::S_INFO);
    }
    const int a;
};
int main(int argc, char ** argv)
{
    cout("hello world!", logger::S_VERBOSE);
    cout("hello world!", logger::S_DEBUG);
    cout("hello world!", logger::S_INFO);
    cout("hello world!", logger::S_WARNING);
    cout("hello world!", logger::S_ERROR);
    cout("hello world!", logger::S_FATAL);

    variant<int, float, stTest, bool> v;
    v.emplace<int>(1);
    cout("v @idx: " + std::to_string(v.index()), logger::S_INFO);
    cout("v is int: " + std::to_string(v.is<int>()) + " @value: " + std::to_string(v.get<int>()) +
         ", v is stTest: " + std::to_string(v.is<stTest>())
         , logger::S_INFO);
    v = 1.0f;
    cout("v @idx: " + std::to_string(v.index()), logger::S_INFO);
    v = false;
    cout("v @idx: " + std::to_string(v.index()), logger::S_INFO);
    cout("v is bool: " + std::to_string(v.is<bool>()) + " @value: " + std::to_string(v.get<bool>())
         , logger::S_INFO);
    v.emplace<stTest>(2);
    cout("v @idx: " + std::to_string(v.index()) + ", stTest::a: " + std::to_string(v.raw_get<stTest>().a),
         logger::S_INFO);
    v = 1;
    cout("v @idx: " + std::to_string(v.index()), logger::S_INFO);
    
    cout.flush();    
    ::getchar();
    return 0;
}
