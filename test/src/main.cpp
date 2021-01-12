#include <chrono>
#include <fstream>
#include <random>
#include <sstream>

#include "src/factory.h"
#include "src/filesystem.h"
#include "src/json.h"
#include "src/logger.h"
#include "src/string.h"
#include "src/variant.h"

using namespace fs::sun;
using namespace std::chrono_literals;

struct TestJson {
  std::string name;
  std::string gender;
  std::uint8_t age{};
  bool ready{};
  int numbers[10]{};

  TestJson *my_friend{nullptr};
  std::unique_ptr<TestJson> my_2nd_friend{nullptr};
  Json::Dictionary misc;
  Json::TDictionary<float> dic_floats{};

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

struct ConcurrentTest {
  int a;
  double b;
  int idx;
  struct Ret {
    double ret;
    std::thread::id t_id;
  };
  mutable std::future<Ret> ret;
  friend std::string to_string(const ConcurrentTest &ct) {
    std::string ret;
    ret += ("idx: " + string::ToString(ct.idx));
    ret += (", a: " + string::ToString(ct.a));
    ret += (", b: " + string::ToString(ct.b));
    if (ct.ret.valid()) {
      if (ct.ret.wait_for(0s) == std::future_status::ready) {
        const Ret ct_ret = ct.ret.get();
        std::stringstream tid_s;
        tid_s << ct_ret.t_id;
        ret += (", ret: " + string::ToString(ct_ret.ret) +
                ", tid: " + tid_s.str());
      }
    }
    return ret;
  }
};

int main(int argc, char **argv) {
  cout("hello world!", Logger::S_VERBOSE);
  cout("hello world!", Logger::S_DEBUG);
  cout("hello world!", Logger::S_INFO);
  cout("hello world!", Logger::S_WARNING);
  cout("hello world!", Logger::S_ERROR);
  cout("hello world!", Logger::S_FATAL);
  cout.Flush();

  cout("exe path: " + Filesystem::Instance().GetExecutablePath(),
       Logger::S_INFO);
  cout("exe dir: " + Filesystem::Instance().GetExecutableDir(),
       Logger::S_INFO);
  cout("wd: " + Filesystem::Instance().GetWorkingDir(), Logger::S_INFO);

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
    TestJson tj;
    tj.ParseFromJsonFile(argv[1]);
    cout(string::ToString(tj), Logger::S_INFO);
  }

  // concurrent
  {
    Concurrent<ConcurrentTest::Ret(const int, const double)> c = {
        [](const int a, const double b) -> ConcurrentTest::Ret {
          ConcurrentTest::Ret ret;
          ret.ret = a + b;
          std::this_thread::sleep_for(50ms);
          ret.t_id = std::this_thread::get_id();
          return ret;
        }};
    std::vector<ConcurrentTest> cts;
    std::random_device rd;
    std::mt19937 mt_gen{rd()};
    std::uniform_int_distribution<> int_distrib{0, 100};
    std::uniform_real_distribution<> real_distrib{.0f, 100.0f};
    for (int i = 0; i < 100; i++) {
      ConcurrentTest ct{int_distrib(mt_gen), real_distrib(mt_gen)};
      ct.idx = i;
      cout(string::ToString(ct));
      ct.ret = c.Push(ct.a, ct.b);
      cts.push_back(std::move(ct));
    }

    c.Run();
    c.Finish();
    for (const auto &ct : cts) {
      cout(string::ToString(ct));
    }
  }

  return 0;
}
