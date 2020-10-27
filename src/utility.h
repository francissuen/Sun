/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_UTILITY_H
#define FS_SUN_UTILITY_H

#include <future>
#include <tuple>
#include <type_traits>

#include "ns.h"

#ifdef _MSC_VER
#ifdef max
#undef max
#endif
#endif

FS_SUN_NS_BEGIN

template <typename T0, typename T1>
T0& Max(const T0& t0, const T1& t1) {
  return t0 > t1 ? t0 : t1;
}

template <typename T0, typename T1, typename T2, typename... Tn>
T0& Max(const T0& t0, const T1& t1, const T2& t2, Tn&... tn) {
  T0& x = Max(tn...);
  return t0 > x ? t0 : x;
}

template <typename T>
void Swap(T& a, T& b) {
  using std::swap;
  swap(a, b);
}

template <typename T, T... n>
struct StaticMax;

template <typename T, T t0, T t1>
struct StaticMax<T, t0, t1> {
  static constexpr T value = t0 > t1 ? t0 : t1;
};

template <typename T, T t0, T t1, T t2, T... tn>
struct StaticMax<T, t0, t1, t2, tn...> {
  static constexpr T x = StaticMax<T, t1, t2, tn...>::value;
  static constexpr T value = t0 > x ? t0 : x;
};

/** index sequence */
template <std::size_t...>
struct IndexSequence {};
template <std::size_t n, std::size_t... s>
struct MakeIndexSequence : MakeIndexSequence<n - 1, n - 1, s...> {};

template <std::size_t... s>
struct MakeIndexSequence<0, s...> {
  using Type = IndexSequence<s...>;
};
template <typename... T>
using IndexSequenceFor = MakeIndexSequence<sizeof...(T)>;

template <typename TFunc, typename TTuple, std::size_t... idx>
typename std::remove_reference<TFunc>::type::result_type Apply(
    TFunc&& f, TTuple&& t, IndexSequence<idx...>) {
  return std::forward<TFunc>(f)(std::get<idx>(std::forward<TTuple>(t))...);
}
template <typename TFunc, typename TTuple>
typename std::remove_reference<TFunc>::type::result_type Apply(TFunc&& f,
                                                               TTuple&& t) {
  return Apply(
      std::forward<TFunc>(f), std::forward<TTuple>(t),
      typename MakeIndexSequence<std::tuple_size<TTuple>::value>::Type{});
}

template <typename TFunc, typename TTuple>
void Apply2Promise(
    TFunc&& f, TTuple&& t,
    std::promise<typename std::enable_if<
        std::is_void<
            typename std::remove_reference<TFunc>::type::result_type>::value,
        typename std::remove_reference<TFunc>::type::result_type>::type>&
        promise) {
  Apply(std::forward<TFunc>(f), std::forward<TTuple>(t));
  promise.set_value();
}

template <typename TFunc, typename TTuple>
void Apply2Promise(
    TFunc&& f, TTuple&& t,
    std::promise<typename std::enable_if<
        !(std::is_void<
            typename std::remove_reference<TFunc>::type::result_type>::value),
        typename std::remove_reference<TFunc>::type::result_type>::type>&
        promise) {
  promise.set_value(Apply(std::forward<TFunc>(f), std::forward<TTuple>(t)));
}

/******************/
/** IndexOf<>In<> */
/******************/
/**
 * \brief find IndexOf<T>::In<Ts...>
 */
template <typename T, typename TIndex = std::uint8_t>
struct IndexOf {
  static constexpr TIndex npos = std::numeric_limits<TIndex>::max();

 private:
  template <TIndex current_index, typename... Ts>
  struct SeekIndex;

  template <TIndex current_index, typename TLast>
  struct SeekIndex<current_index, TLast> {
    static constexpr TIndex index =
        std::is_same<T, TLast>::value ? current_index : npos;
  };

  template <TIndex current_index, typename T0, typename T1, typename... Tn>
  struct SeekIndex<current_index, T0, T1, Tn...> {
    static constexpr TIndex index =
        std::is_same<T, T0>::value
            ? current_index
            : SeekIndex<current_index + 1u, T1, Tn...>::index;
  };

 public:
  template <typename... Ts>
  struct In {
    static_assert(sizeof...(Ts) > 0, "count of Ts should be at least one.");
    static_assert(sizeof...(Ts) < npos, "too many Ts... for type TIndex");
    static constexpr TIndex value = SeekIndex<0u, Ts...>::index;
  };
};

template <typename T, typename TIndex>
template <typename... Ts>
constexpr TIndex IndexOf<T, TIndex>::In<Ts...>::value;

/*******************/
/** TypeOf<>::In<> */
/*******************/
/**
 * \brief find TypeOf<index>::In<Ts...>
 */
template <std::size_t index>
struct TypeOf {
 private:
  template <std::size_t cur_index, typename... Ts>
  struct SeekType {
    static_assert(sizeof...(Ts) > 0, "count of Ts should be at least one");
  };

  template <std::size_t cur_index, typename TLast>
  struct SeekType<cur_index, TLast> {
    static_assert(index == cur_index, "No specified position type found.");
    using Type = TLast;
  };

  template <std::size_t cur_index, typename T0, typename T1, typename... Tn>
  struct SeekType<cur_index, T0, T1, Tn...> {
    using Type = typename std::conditional<
        index == cur_index, T0,
        typename SeekType<cur_index + 1, T1, Tn...>::Type>::type;
  };

 public:
  template <typename... Ts>
  struct In {
    static_assert(sizeof...(Ts) > 0, "count of Ts should be at least one");
    using Type = typename SeekType<0u, Ts...>::Type;
  };
};

/**
 * \brief Invoke<TFunctor>::SequentiallyFor<Ts...>::With(args...)
 */
template <template <typename> class TFunctor>
struct Invoke {
  template <typename... Ts>
  struct ForTypeIn;

  template <typename TLast>
  struct ForTypeIn<TLast> {
    template <typename... TArgs>
    static void With(TArgs&&... args) {
      TFunctor<TLast> func;
      func(std::forward<TArgs>(args)...);
    }

    template <typename TIndex, typename... TArgs>
    static void With2Internal(const TIndex target_index,
                              const TIndex current_index, TArgs&&... args) {
      if (target_index == current_index) {
        TFunctor<TLast> func;
        func(std::forward<TArgs>(args)...);
      }
    }
  };

  template <typename T0, typename T1, typename... TOthers>
  struct ForTypeIn<T0, T1, TOthers...> {
    template <typename... TArgs>
    static void With(TArgs&&... args) {
      ForTypeIn<T0>::With(std::forward<TArgs>(args)...);
      ForTypeIn<T1, TOthers...>::With(std::forward<TArgs>(args)...);
    }

    template <typename TIndex, typename... TArgs>
    static void With2Internal(const TIndex target_index,
                              const TIndex current_index, TArgs&&... args) {
      if (target_index == current_index)
        ForTypeIn<T0>::With(std::forward<TArgs>(args)...);
      else
        ForTypeIn<T1, TOthers...>::template With2Internal<TIndex, TArgs...>(
            target_index, current_index + 1u, std::forward<TArgs>(args)...);
    }

    template <typename TIndex, typename... TArgs>
    static void With2(const TIndex target_index, TArgs&&... args) {
      With2Internal<TIndex, TArgs...>(target_index, 0u,
                                      std::forward<TArgs>(args)...);
    }
  };
};

/**
 * \brief value = (type_t<Ts>::value &&)...
 */
template <template <typename> class TValue, typename... Ts>
struct StaticAnd;
template <template <typename> class TValue, typename T0, typename T1,
          typename... Tn>
struct StaticAnd<TValue, T0, T1, Tn...> {
  static constexpr bool value =
      TValue<T0>::value && StaticAnd<TValue, T1, Tn...>::value;
};
template <template <typename> class TValue, typename TLast>
struct StaticAnd<TValue, TLast> {
  static constexpr bool value = TValue<TLast>::value;
};

/**
 * \brief IsType<T>::In<Ts...>::value
 */
template <typename T, typename TIndex = std::uint8_t>
struct IsType {
  template <typename... Ts>
  struct In {
    static constexpr bool value =
        IndexOf<T>::template In<Ts...>::value != IndexOf<T>::npos;
  };
};

template <typename T, std::size_t N>
constexpr std::size_t CountOfArray(T (&)[N]) {
  return N;
}

template <typename T>
struct remove_cvref {
  using type =
      typename std::remove_cv<typename std::remove_reference<T>::type>::type;
};

template <typename T>
struct WhichType;

FS_SUN_NS_END

#endif  // FS_SUN_UTILITY_H