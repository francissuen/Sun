/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include <type_traits>
#include <tuple>
#include <future>

FS_SUN_NS_BEGIN

template<typename T0, typename T1>
T0 & Max(const T0 & t0, const T1 & t1)
{
    return t0 > t1? t0 : t1;
}

template<typename T0, typename T1, typename T2, typename ... Tn>
T0 & Max(const T0 & t0, const T1 & t1, const T2 & t2, Tn & ... tn)
{
    T0 & x = Max(tn...);
    return t0 > x? t0 : x;
}

template<typename T, T ... n>
struct StaticMax;

template<typename T, T t0, T t1>
struct StaticMax<T, t0, t1>
{
    static constexpr T value = t0 > t1? t0 : t1;
};

template<typename T, T t0, T t1, T t2, T ... tn>
struct StaticMax<T, t0, t1, t2, tn ...>
{
    static constexpr T x = StaticMax<T, t1, t2, tn ...>::value;
    static constexpr T value = t0 > x? t0 : x;
};

/** index sequence */
template <std::size_t ...>
struct IndexSequence
{};
template <std::size_t n, std::size_t ... s>
struct MakeIndexSequence : MakeIndexSequence<n-1, n-1, s...>
{};

template <std::size_t ... s>
struct MakeIndexSequence<0, s...>
{
    using Type = IndexSequence<s...>;
};
template <typename ... T>
using IndexSequenceFor = MakeIndexSequence<sizeof...(T)>;

template <typename TFunc, typename TTuple, std::size_t ... idx>
typename std::remove_reference<TFunc>::type::result_type
Apply(TFunc && f, TTuple && t, IndexSequence<idx...>)
{
    return std::forward<TFunc>(f)(std::get<idx>(std::forward<TTuple>(t))...);
}
template <typename TFunc, typename TTuple>
typename std::remove_reference<TFunc>::type::result_type
Apply(TFunc && f, TTuple && t)
{
    return Apply(std::forward<TFunc>(f), std::forward<TTuple>(t),
                 typename MakeIndexSequence<
                 std::tuple_size<TTuple>::value>::Type{});
}

template <typename TFunc, typename TTuple>
void Apply2Promise(TFunc && f, TTuple && t,
                   std::promise<typename
                   std::enable_if<std::is_void<
                   typename std::remove_reference<
                   TFunc>::type::result_type>::value, typename std::remove_reference<
                   TFunc>::type::result_type>::type> & promise)
{
    Apply(std::forward<TFunc>(f), std::forward<TTuple>(t));
    promise.set_value();
}

template <typename TFunc, typename TTuple>
void Apply2Promise(TFunc && f, TTuple && t,
                   std::promise<typename
                   std::enable_if<! (std::is_void<typename std::remove_reference<
                                     TFunc>::type::result_type>::value),
                   typename std::remove_reference<
                   TFunc>::type::result_type>::type> & promise)
{
    promise.set_value(Apply(std::forward<TFunc>(f), std::forward<TTuple>(t)));
}


/******************/
/** IndexOf<>In<> */
/******************/
/**
 * \brief find IndexOf<T>::In<Ts...>
 */
template<typename T, typename TIndex = std::uint8_t>
struct IndexOf
{
private:
    static constexpr TIndex invalid_count = std::numeric_limits<TIndex>::max();
    template<typename ... Ts>
    struct WalkUntilFound
    {
        static_assert(sizeof...(Ts) > 0, "count of Ts should be at least one.");
    };
    
    template<typename TLast>
    struct WalkUntilFound<TLast>
    {
        static constexpr TIndex count_of_left_step = std::is_same<T, TLast>::value?
        0 : invalid_count;
    };

    template<typename T0, typename T1, typename ... Tn>
    struct WalkUntilFound<T0, T1, Tn...>
    {
        static constexpr TIndex count_of_left_step = std::is_same<T, T0>::value?
        sizeof...(Tn) + 1 : WalkUntilFound<T1, Tn...>::count_of_left_step;
    };
    
public:
    static constexpr TIndex npos = std::numeric_limits<TIndex>::max();
    template<typename ... Ts>
    struct In
    {
        static_assert(sizeof...(Ts) > 0, "count of Ts should be at least one.");
        static constexpr TIndex value = WalkUntilFound<Ts...>::count_of_left_step != invalid_count?
        sizeof...(Ts) - WalkUntilFound<Ts...>::count_of_left_step - 1: npos;
    };
};


template<typename T, typename TIndex>
template<typename ... Ts>
constexpr TIndex IndexOf<T, TIndex>::In<Ts...>::value;

/*******************/
/** TypeOf<>::In<> */
/*******************/
/**
 * \brief find TypeOf<index>::In<Ts...>
 */
template<std::size_t index>
struct TypeOf
{
private:
    template<std::size_t cur_index, typename ... Ts>
    struct WalkUntilFound
    {
        static_assert(sizeof...(Ts) > 0, "count of Ts should be at least one");        
    };

    template<std::size_t cur_index, typename TLast>
    struct WalkUntilFound<cur_index, TLast>
    {
        static_assert(index == cur_index, "No specified position type found.");
        using Type = TLast;
    };

    template<std::size_t cur_index, typename T0, typename T1, typename ... Tn>
    struct WalkUntilFound<cur_index, T0, T1, Tn...>
    {
        using Type = typename std::conditional<index == cur_index, T0,
                                               typename WalkUntilFound<cur_index + 1,
                                                                       T1, Tn...>::Type>::type;
    };
public:
    template<typename ... Ts>
    struct In
    {
        static_assert(sizeof...(Ts) > 0, "count of Ts should be at least one");
        using Type = typename WalkUntilFound<0u, Ts...>::Type;
    };
};

/**
 * \brief Invoke<functor_t<T>>::For<Ts...>::With(args...)
 */
template<template<typename> class TFunctor>
struct Invoke
{
    template<typename ... Ts>
    struct For;

    template<typename TLast>
    struct For<TLast>
    {
        template<typename ... TArgs>
        static void With(TArgs ... args)
        {
            TFunctor<TLast> func;
            func(args...);
        }
    };

    template<typename T0, typename T1, typename ... Tn>
    struct For<T0, T1, Tn...>
    {
        template<typename ... TArgs>
        static void With(TArgs ... args)
        {
            For<T0>::With(args...);
            For<T1, Tn...>::With(args...);
        }
    };
};
/** 
 * \brief value = (type_t<Ts>::value &&)...
 */
template<template<typename> class TValue, typename ... Ts>
struct StaticAnd;
template<template<typename> class TValue, typename T0, typename T1, typename ... Tn>
struct StaticAnd<TValue, T0, T1, Tn ...>
{
    static constexpr bool value = TValue<T0>::value && StaticAnd<TValue, T1, Tn ...>::value;
};
template<template<typename> class TValue, typename TLast>
struct StaticAnd<TValue, TLast>
{
    static constexpr bool value = TValue<TLast>::value;
};


/**
 * \brief IsType<T>::In<Ts...>::value
 */
template<typename T, typename TIndex = std::uint8_t>
struct IsType
{
    template<typename ... Ts>
    struct In
    {
        static constexpr bool value = IndexOf<T>::template In<Ts...>::value != IndexOf<T>::npos;
    };
};

FS_SUN_NS_END
