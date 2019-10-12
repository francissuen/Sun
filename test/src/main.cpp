#include "src/logger.h"
#include "src/string.h"
#include "src/factory.h"
#include "src/string.h"
#include "src/json.h"
#include <fstream>
#include <sstream>

using namespace fs::sun;

struct stTest
{
    stTest(const int _a):
        a(_a)
    {
        cout("stTest called @a: " + string::ToString(a), Logger::S_INFO);
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
    
    constexpr char const * test_json =
    "{"
    "\"name\":\"fs\","
    "\"gender\":\"male\""
    "}";
    
    if(argc > 1)
    {
        std::ifstream json_file(argv[1]);
        std::string json_data;
        if(json_file.is_open())
        {
            std::stringstream json_stream;
            json_file >> json_stream.rdbuf();
            json_data = json_stream.str();
        }

        Json j(json_data.c_str());
        if(j.Initialize())
        {
            cout(string::ToString(j.GetVariables()), Logger::S_INFO);
        }
        else
            cout("json initialize failed.", Logger::S_ERROR);

    }

    cout.Flush();
    
    return 0;
}
