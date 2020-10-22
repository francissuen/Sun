#include <fstream>
#include <sstream>

#include "src/factory.h"
#include "src/json.h"
#include "src/logger.h"
#include "src/string.h"
#include "src/variant.h"

using namespace fs::sun;

struct TestJson {
  std::string name;
  std::string gender;
  std::uint8_t age{};
  bool ready{};
  int numbers[10]{};

  TestJson *my_friend{nullptr};
  std::unique_ptr<TestJson> my_2nd_friend{nullptr};
  Json misc;
  Json::Dictionary<float> dic_floats{};

  FS_SUN_JSON_REGISTER_OBJECT_BEGIN()
  FS_SUN_JSON_REGISTER_OBJECT_MEMBER(name)
  FS_SUN_JSON_REGISTER_OBJECT_MEMBER(gender)
  FS_SUN_JSON_REGISTER_OBJECT_MEMBER(age)
  FS_SUN_JSON_REGISTER_OBJECT_MEMBER(ready)
  FS_SUN_JSON_REGISTER_OBJECT_MEMBER(numbers)
  FS_SUN_JSON_REGISTER_OBJECT_MEMBER(my_friend)
  FS_SUN_JSON_REGISTER_OBJECT_MEMBER(my_2nd_friend)
  FS_SUN_JSON_REGISTER_OBJECT_MEMBER(misc)
  FS_SUN_JSON_REGISTER_OBJECT_MEMBER(dic_floats)
  FS_SUN_JSON_REGISTER_OBJECT_END()

  friend std::string to_string(const TestJson &tj) {
    std::string ret{"{name: "};
    ret += tj.name + ", gender: " + tj.gender +
           ", age: " + string::ToString(tj.age) +
           ", ready: " + string::ToString(tj.ready) +
           ", numbers: " + string::ToString(tj.numbers);

    if (tj.my_friend != nullptr)
      ret += (", my_friend: " + string::ToString(*(tj.my_friend)));
    if (tj.my_2nd_friend != nullptr)
      ret += (", my_2nd_friend " + string::ToString(*(tj.my_2nd_friend)));

    ret += (", misc: " + string::ToString(tj.misc));

    ret += ", dic_floats: " + string::ToString(tj.dic_floats);
    ret += "}";
    return ret;
  }
};

struct B {
  static constexpr char const *name = "B";
  B(int) { FS_SUN_LOG("B cotr", Logger::S_INFO); }
};
struct C {
  static constexpr char const *name = "C";
  C(int) { FS_SUN_LOG("C cotr", Logger::S_INFO); }
};
int main(int argc, char **argv) {
  cout("hello world!", Logger::S_VERBOSE);
  cout("hello world!", Logger::S_DEBUG);
  cout("hello world!", Logger::S_INFO);
  cout("hello world!", Logger::S_WARNING);
  cout("hello world!", Logger::S_ERROR);
  cout("hello world!", Logger::S_FATAL);
  cout.Flush();

  Factory<std::string, int> f;
  f.Register<B>(B::name);
  f.Register<C>(C::name);
  std::unique_ptr<B> b = f.Create<B>(B::name, 1);
  std::unique_ptr<C> c = f.Create<C>(C::name, 0);
  f.Unregister(B::name);
  f.Unregister(C::name);

  Variant<int, bool, std::string> v;
  v = 1;
  cout(string::ToString(v.Get<int>()), Logger::S_INFO);
  v = false;
  cout(string::ToString(v.Get<bool>()), Logger::S_INFO);
  v = std::string("a");
  cout(string::ToString(v), Logger::S_INFO);

  if (argc > 1) {
    cout(argv[1], Logger::S_INFO);
    std::ifstream json_file(argv[1]);
    std::string json_data;
    if (json_file.is_open()) {
      std::stringstream json_stream;
      json_file >> json_stream.rdbuf();
      json_data = json_stream.str();

      TestJson tj;
      tj.ParseFromJson(json_data.c_str(), json_data.size());
      cout(string::ToString(tj), Logger::S_INFO);
    }
  }

  return 0;
}
