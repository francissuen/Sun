#include "src/logger.h"
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

    struct A{};
    struct B: public A{B(int){}};
    struct C: public A{C(int){}};

    const Factory<A, int> f = Factory<A, int>::With<B, C>();
    std::unique_ptr<A> a = f.Create(Factory<A, int>::With<B, C>::OrderNumberOf<B>(), 1);
    
    ::getchar();
    return 0;
}
