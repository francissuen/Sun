#include "src/logger.h"
#include "src/variant.h"
#include "src/string.h"
#include "src/factory.h"
#include "src/string.h"

using namespace fs::sun;

struct stTest
{
    stTest(const int _a):
        a(_a)
    {
        cout("stTest called @a: " + std::to_string(a), Logger::S_INFO);
    }
    ~stTest()
    {
        cout("~stTest called.", Logger::S_INFO);
    }
    const int a;
};

int main(int argc, char ** argv)
{
    cout("hello world!", Logger::S_VERBOSE);
    cout("hello world!", Logger::S_DEBUG);
    cout("hello world!", Logger::S_INFO);
    cout("hello world!", Logger::S_WARNING);
    cout("hello world!", Logger::S_ERROR);
    cout("hello world!", Logger::S_FATAL);

    Variant<int, float, stTest, bool> v;
    v.Emplace<int>(1);
    cout("v @idx: " + std::to_string(v.Index()), Logger::S_INFO);
    cout("v is int: " + std::to_string(v.Is<int>()) + " @value: " + std::to_string(v.Get<int>()) +
         ", v is stTest: " + std::to_string(v.Is<stTest>())
         , Logger::S_INFO);
    v = 1.0f;
    cout("v @idx: " + std::to_string(v.Index()), Logger::S_INFO);
    v = false;
    cout("v @idx: " + std::to_string(v.Index()), Logger::S_INFO);
    cout("v is bool: " + std::to_string(v.Is<bool>()) + " @value: " + std::to_string(v.Get<bool>())
         , Logger::S_INFO);
    v.Emplace<stTest>(2);
    cout("v @idx: " + std::to_string(v.Index()) + ", stTest::a: "
         + std::to_string(v.RawGet<stTest>().a),
         Logger::S_INFO);
    v = 1;
    cout("v @idx: " + std::to_string(v.Index()), Logger::S_INFO);
    
    cout.Flush();

    struct A{};
    struct B: public A{B(int){}};
    struct C: public A{C(int){}};

    const Factory<A, int> f = Factory<A, int>::With<B, C>();
    std::unique_ptr<A> a = f.Create(Factory<A, int>::With<B, C>::OrderNumberOf<B>(), 1);
    
    ::getchar();
    return 0;
}
