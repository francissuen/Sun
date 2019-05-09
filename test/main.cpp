#include "../src/logger.h"
#include "../src/variant.h"

using namespace fs::Sun;

struct stTest
{
    ~stTest()
    {
        cout("~stTest called.", logger::S_INFO);
    }
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
    v = 1;
    cout("v @idx: " + std::to_string(v.index()), logger::S_INFO);
    v = 1;
    cout("v @idx: " + std::to_string(v.index()), logger::S_INFO);
    v = 1.0f;
    cout("v @idx: " + std::to_string(v.index()), logger::S_INFO);
    v = false;
    cout("v @idx: " + std::to_string(v.index()), logger::S_INFO);
    v = stTest();
    cout("v @idx: " + std::to_string(v.index()), logger::S_INFO);
    v = 1;
    cout("v @idx: " + std::to_string(v.index()), logger::S_INFO);
    
    cout.flush();    
    ::getchar();
    return 0;
}
