#include "src/compile_time.h"
#include "src/factory.h"
#include "src/filesystem.h"
#include "src/id.h"
#include "src/json.h"
#include "src/logger.h"
#include "src/string.h"

#include <chrono>
#include <fstream>
#include <random>
#include <sstream>
#include <unordered_map>

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

  // FS_SUN_JSON_REGISTER_OBJECT_BEGIN()
  // FS_SUN_JSON_REGISTER_OBJECT_MEMBER(name)
  // FS_SUN_JSON_REGISTER_OBJECT_MEMBER(gender)
  // FS_SUN_JSON_REGISTER_OBJECT_MEMBER(age)
  // FS_SUN_JSON_REGISTER_OBJECT_MEMBER(ready)
  // FS_SUN_JSON_REGISTER_OBJECT_MEMBER(numbers)
  // FS_SUN_JSON_REGISTER_OBJECT_MEMBER(my_friend)
  // FS_SUN_JSON_REGISTER_OBJECT_MEMBER(my_2nd_friend)
  // FS_SUN_JSON_REGISTER_OBJECT_MEMBER(misc)
  // FS_SUN_JSON_REGISTER_OBJECT_MEMBER(dic_floats)
  // FS_SUN_JSON_REGISTER_OBJECT_END()
  FS_SUN_JSON_REGISTER(name, gender, age, ready, numbers, my_friend,
                       my_2nd_friend, misc, dic_floats)

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
  B(int) { FS_SUN_INFO("B cotr"); }
};
struct C {
  static constexpr char const *name = "C";
  C(int) { FS_SUN_INFO("C cotr"); }
};

struct AsyncBatchedTest {
  int a;
  double b;
  int idx{};
  struct Ret {
    double ret{};
    std::size_t t_id{};
  };
  mutable std::future<Ret> ret{};
  friend std::string to_string(const AsyncBatchedTest &ct) {
    std::string ret;
    ret += ("idx: " + string::ToString(ct.idx));
    ret += (", a: " + string::ToString(ct.a));
    ret += (", b: " + string::ToString(ct.b));
    if (ct.ret.valid()) {
      if (ct.ret.wait_for(0s) == std::future_status::ready) {
        const Ret ct_ret = ct.ret.get();
        ret += (", ret: " + string::ToString(ct_ret.ret) +
                ", tid: " + string::ToString(ct_ret.t_id));
      }
    }
    return ret;
  }
};

int main(int argc, char **argv) {
  auto &logger = Logger<>::Instance();
  logger.Log("hello world!", logger::S_VERBOSE);
  logger.Log("hello world!", logger::S_DEBUG);
  logger.Log("hello world!", logger::S_INFO);
  logger.Log("hello world!", logger::S_WARNING);
  logger.Log("hello world!", logger::S_ERROR);
  logger.Log("hello world!", logger::S_FATAL);
  logger.Flush();

  logger.Log("exe path: " + Filesystem::Instance().GetExecutablePath(),
             logger::S_INFO);
  logger.Log("exe dir: " + Filesystem::Instance().GetExecutableDir(),
             logger::S_INFO);
  logger.Log("wd: " + Filesystem::Instance().GetWorkingDir(), logger::S_INFO);

  Factory<std::string, int> f;
  f.Register<B>(B::name);
  f.Register<C>(C::name);
  std::unique_ptr<B> b = f.Create<B>(B::name, 1);
  std::unique_ptr<C> c = f.Create<C>(C::name, 0);
  f.Unregister(B::name);
  f.Unregister(C::name);

  if (argc > 1) {
    logger.Log(argv[1], logger::S_INFO);
    TestJson tj;
    if (tj.ParseFromJsonFile(argv[1]))
      logger.Log(string::ToString(tj), logger::S_INFO);
    else
      logger.Log("Failed parsing TestJson file", logger::S_ERROR);
  }

  // concurrent
  {
    AsyncBatched<AsyncBatchedTest::Ret(const ThreadIndex thread_idx, const int,
                                       const double)>
        ab = {[](const ThreadIndex thread_idx, const int a,
                 const double b) -> AsyncBatchedTest::Ret {
          AsyncBatchedTest::Ret ret;
          ret.ret = a + b;
          std::this_thread::sleep_for(50ms);
          ret.t_id = thread_idx.idx;
          return ret;
        }};
    std::vector<AsyncBatchedTest> abts;
    std::random_device rd;
    std::mt19937 mt_gen{rd()};
    std::uniform_int_distribution<> int_distrib{0, 100};
    std::uniform_real_distribution<> real_distrib{.0f, 100.0f};

    for (int batch = 0; batch < 2; batch++) {
      for (int i = 0; i < 80; i++) {
        AsyncBatchedTest abt{int_distrib(mt_gen), real_distrib(mt_gen)};
        abt.idx = i;
        logger.Log(string::ToString(abt));
        abt.ret = ab.Add(abt.a, abt.b);
        abts.push_back(std::move(abt));
      }
      ab.Commit();
    }

    ab.Finish();
    for (const auto &abt : abts) {
      logger.Log(string::ToString(abt));
    }
  }

  using MyCTMap = compile_time::Map<std::unordered_map<int, int>, CTPair(1, 2),
                                    CTPair(2, 3)>;
  static_assert(MyCTMap::Get<1>::value == 2);
  static_assert(MyCTMap::Get<3, -1>::value == -1);
  assert(MyCTMap::GetRTMap().find(2)->second == MyCTMap::Get<2>::value);

  return 0;
}
