#include "src/logger.h"
#include "src/string.h"
#include "src/factory.h"
#include "src/string.h"
#include "src/json.h"
#include <fstream>
#include <sstream>
#include "src/variant.h"


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

struct TestJson
{
    std::string name;
    std::string gender;
    std::uint8_t age;
    bool ready;
    int numbers[10];

    FS_SUN_JSON_REGISTER_OBJECT_BEGIN(TestJson)
    FS_SUN_JSON_REGISTER_OBJECT_MEMBER(name)
    FS_SUN_JSON_REGISTER_OBJECT_MEMBER(gender)
    FS_SUN_JSON_REGISTER_OBJECT_MEMBER(age)
    FS_SUN_JSON_REGISTER_OBJECT_MEMBER(ready)
    FS_SUN_JSON_REGISTER_OBJECT_MEMBER(numbers)
    FS_SUN_JSON_REGISTER_OBJECT_END()

    std::string ToString() const
    {
        std::string ret{"@name: "};
        ret += name + ", @gender: " + gender + ", @age: " + string::ToString(age) +
        ", @ready: " + string::ToString(ready) + ", @numbers: ";
        for(std::size_t i = 0u; i < 10u; i++)
        {
            ret += string::ToString(numbers[i]) + ", ";
        }
        return ret;
    }
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

    Variant<int, bool, std::string> v;
    /** v = 1; */
    /** cout(string::ToString(v.Get<int>()), Logger::S_INFO); */
    /** v = true; */
    /** cout(string::ToString(v.Get<bool>()), Logger::S_INFO); */
    v = std::string("1");
    cout(v.ToString(), Logger::S_INFO);
         
    if(argc > 1)
    {
        cout(argv[1], Logger::S_INFO);
        std::ifstream json_file(argv[1]);
        std::string json_data;
        if(json_file.is_open())
        {
            std::stringstream json_stream;
            json_file >> json_stream.rdbuf();
            json_data = json_stream.str();

            TestJson tj;
            tj.ParseFromJson(json_data.c_str(), json_data.size());
            cout(tj.ToString().c_str(), Logger::S_INFO);
        }
    }

    cout.Flush();

    return 0;
}
