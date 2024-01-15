#pragma once

#include <type_traits>
#include <unordered_map>

namespace fs {
namespace sun {
namespace compile_time {

/**
 * @brief compile time map, which can get the value by key during the compiling
 * time.
 */
template <typename... TPairs>
struct Map;

template <typename TKey, typename TValue, TKey t_key, TValue t_value>
struct Pair : std::pair<TKey, TValue> {
  using Key = TKey;
  using Value = TValue;

  Pair() : std::pair<TKey, TValue>{t_key, t_value} {}
};

template <typename T>
struct IsPair {
  static constexpr bool value = false;
};
template <typename TKey, typename TValue, TKey t_key, TValue t_value>
struct IsPair<Pair<TKey, TValue, t_key, t_value>> {
  static constexpr bool value = true;
};

template <typename TKey, typename TValue, TKey t_key, TValue t_value>
struct Map<Pair<TKey, TValue, t_key, t_value>> {
  template <TKey t_in_key, TValue... value>
  struct Get;
  template <TKey t_in_key>
  struct Get<t_in_key> {
    static constexpr TValue value = t_value;
    static constexpr bool has_key = t_in_key == t_key;
  };

  template <TKey t_in_key, TValue default_value>
  struct Get<t_in_key, default_value> {
    static constexpr TValue value =
        t_in_key == t_key ? t_value : default_value;
  };
};

template <typename TKey, typename TValue, TKey t_key, TValue t_value,
          typename... TOtherPairs>
struct Map<Pair<TKey, TValue, t_key, t_value>, TOtherPairs...> {
  template <TKey t_in_key, TValue... value>
  struct Get;
  template <TKey t_in_key>
  struct Get<t_in_key> {
    static constexpr TValue value =
        t_in_key == t_key ? t_value
                          : Map<TOtherPairs...>::template Get<t_in_key>::value;
    static constexpr bool has_key =
        t_in_key == t_key
            ? true
            : Map<TOtherPairs...>::template Get<t_in_key>::has_key;
    static_assert(has_key, "No specified key found.");
  };

  template <TKey t_in_key, TValue default_value>
  struct Get<t_in_key, default_value> {
    static constexpr TValue value =
        t_in_key == t_key
            ? t_value
            : Map<TOtherPairs...>::template Get<t_in_key,
                                                default_value>::value;
  };
};

/**
 * @brief Support for runtime map at the same time.
 *
 * @tparam TRTMap type of runtime map
 * @tparam TKey
 * @tparam TValue
 * @tparam t_key
 * @tparam t_value
 * @tparam TOtherPairs
 */
template <typename TRTMap, typename TKey, typename TValue,
          typename std::enable_if<!(IsPair<TRTMap>::value), TKey>::type t_key,
          TValue t_value, typename... TOtherPairs>
struct Map<TRTMap, Pair<TKey, TValue, t_key, t_value>, TOtherPairs...> {
  static const TRTMap& GetRTMap() {
    static const TRTMap runtime_map{Pair<TKey, TValue, t_key, t_value>{},
                                    TOtherPairs{}...};
    return runtime_map;
  }

  template <TKey t_in_key, TValue... value>
  struct Get;
  template <TKey t_in_key>
  struct Get<t_in_key> {
    static constexpr TValue value =
        Map<Pair<TKey, TValue, t_key, t_value>,
            TOtherPairs...>::template Get<t_in_key>::value;
  };

  template <TKey t_in_key, TValue default_value>
  struct Get<t_in_key, default_value> {
    static constexpr TValue value =
        Map<Pair<TKey, TValue, t_key, t_value>,
            TOtherPairs...>::template Get<t_in_key, default_value>::value;
  };
};

#define CTPair(k, v) \
  fs::sun::compile_time::Pair<decltype(k), decltype(v), k, v>

template <typename T, T... n>
struct Max;

template <typename T, T t0, T t1>
struct Max<T, t0, t1> {
  static constexpr T value = t0 > t1 ? t0 : t1;
};

template <typename T, T t0, T t1, T t2, T... tn>
struct Max<T, t0, t1, t2, tn...> {
  static constexpr T x = Max<T, t1, t2, tn...>::value;
  static constexpr T value = t0 > x ? t0 : x;
};

}  // namespace compile_time

}  // namespace sun

}  // namespace fs
