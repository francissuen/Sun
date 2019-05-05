/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include <cassert>
#include <iostream>
#include <type_traits>
#include <tuple>
#include <future>

#include "ns.h"

///////////////////////
// Sun useful macros //
///////////////////////

#define FS_SUN_FRIEND_BINARY_OPERATOR_DECLARE(return_t, operator_, operand_1_t, operand_2_t) \
    friend return_t operator operator_ (operand_1_t, operand_2_t);      \
    friend return_t operator operator_ (operand_2_t, operand_1_t);      \

//iff it's commutative
#define FS_SUN_FRIEND_BINARY_OPERATOR_DECLARE_SYMMETRICALLY(return_t, operator_, operand_1_t, operand_2_t) \
    friend return_t operator operator_ (operand_1_t, operand_2_t);      \
    inline friend return_t operator operator_ (operand_2_t o2, operand_1_t o1) \
    {                                                                   \
        return o1 operator_  o2;                                        \
    }

#define FS_SUN_SAFE_DELETE_ARRAY(x)             \
    delete [] x;                                \
    x = nullptr;                                \

#define FS_SUN_SAFE_DELETE(x)                   \
    delete x;                                   \
    x = nullptr;                                \

// using variadic macro for the type contains comma, such as std::map<int, int>
#define FS_SUN_PROPERTY_R(acc_spec, name, ...)                          \
    acc_spec:    __VA_ARGS__ _##name;                                   \
public:                                                                 \
inline __VA_ARGS__ const & Get##name()const&                            \
{                                                                       \
    return _##name;                                                     \
}                                                                       \
inline __VA_ARGS__& Get##name()&                                        \
{                                                                       \
    return _##name;                                                     \
}                                                                       \
inline __VA_ARGS__&& Get##name()&&                                      \
{                                                                       \
    return std::move(_##name);                                          \
}                                                                       \
private:                        // back to default access specifier

#define FS_SUN_PROPERTY_W(acc_spec, name, ...)  \
    acc_spec: __VA_ARGS__ _##name;              \
public:                                         \
inline void Set##name(__VA_ARGS__ const & val)  \
{                                               \
    _##name = val;                              \
}                                               \
inline void Set##name(__VA_ARGS__&& val)        \
{                                               \
    _##name = std::move(val);                   \
}                                               \
private:


#define FS_SUN_PROPERTY(acc_spec, name, ...)    \
    acc_spec: __VA_ARGS__ _##name;              \
public:                                         \
inline void Set##name(__VA_ARGS__ const & val)  \
{                                               \
    _##name = val;                              \
}                                               \
inline void Set##name(__VA_ARGS__&& val)        \
{                                               \
    _##name = std::move(val);                   \
}                                               \
inline __VA_ARGS__ const & Get##name()const&    \
{                                               \
    return _##name;                             \
}                                               \
inline __VA_ARGS__& Get##name()&                \
{                                               \
    return _##name;                             \
}                                               \
inline __VA_ARGS__&& Get##name()&&              \
{                                               \
    return std::move(_##name);                  \
}                                               \
private:

#define FS_SUN_EXPAND(...) __VA_ARGS__
#define FS_SUN_STRING(a) #a
#define FS_SUN_MERGE(a,    b) a##b
#define FS_SUN_MACRO_CALL(func,  param) func param

#ifdef _MSC_VER
#define FS_SUN_MERGE_AFTER_EXPAND(a, b) FS_SUN_MACRO_CALL(FS_SUN_MERGE, (a, b))
#else
#define FS_SUN_MERGE_AFTER_EXPAND(a, b) FS_SUN_MERGE(a, b)
#endif

#define _FS_SUN_EXCLUDE_FIRST_ARG_(first, ...) __VA_ARGS__
#if defined(_MSC_VER) && ! defined(__clang__)
//MS preprocessor issue. see https://stackoverflow.com/questions/48088834/how-to-implement-exclude-first-argument-macro-in-msvc
#define FS_SUN_EXCLUDE_FIRST_ARG(...)     FS_SUN_MACRO_CALL(_FS_SUN_EXCLUDE_FIRST_ARG_, (__VA_ARGS__))
#else
#define FS_SUN_EXCLUDE_FIRST_ARG(...)     _FS_SUN_EXCLUDE_FIRST_ARG_(__VA_ARGS__)
#endif

// trailing comma suppressing
//https://msdn.microsoft.com/en-us/library/ms177415.aspx
//https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
#ifdef _MSC_VER
#define FS_SUN_COMMA__VA_ARGS__(...)  , __VA_ARGS__
#else
#define FS_SUN_COMMA__VA_ARGS__(...) , ##__VA_ARGS__
#endif

#define _FS_SUN_GET32TH_ARGS_(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,  \
                              _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                              _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
                              _31, n, ...) n
    
/** expand args before call _FS_SUN_GET32TH_ARGS_ */    
#if defined(_MSC_VER) && ! defined(__clang__)
#define FS_SUN_GET32TH_ARGS(...) FS_SUN_MACRO_CALL(_FS_SUN_GET32TH_ARGS_, (__VA_ARGS__))
#else
#define FS_SUN_GET32TH_ARGS(...) _FS_SUN_GET32TH_ARGS_(__VA_ARGS__)
#endif

#define FS_SUN_GET32TH_START_FROM_2ND(...) FS_SUN_GET32TH_ARGS(FS_SUN_EXCLUDE_FIRST_ARG(__VA_ARGS__))

#define _FS_SUN_SPAWN_31_1_(_31, _1)  _31, _31, \
        _31, _31, _31, _31, _31,                \
        _31, _31, _31, _31, _31,                \
        _31, _31, _31, _31, _31,                \
        _31, _31, _31, _31, _31,                \
        _31, _31, _31, _31, _31,                \
        _31, _31, _31, _31, _1                  

#define FS_SUN_COMMA ,
#define FS_SUN_EMPTY

/**
 * \brief Expand to a comma(\b ,) when the count of variadic arguments greater than 0.
 */
#define FS_SUN_SMART_COMMA(...) FS_SUN_MERGE_AFTER_EXPAND(FS, FS_SUN_GET32TH_START_FROM_2ND(FS_SUN_COMMA__VA_ARGS__(__VA_ARGS__), _FS_SUN_SPAWN_31_1_(_SUN_COMMA, _SUN_EMPTY)))

/**
 * \brief Expand to \b param only when the count of variadic arguments greater than 0.
 */
#define FS_SUN_SMART_PARAM(param, ...) FS_SUN_GET32TH_START_FROM_2ND(FS_SUN_COMMA__VA_ARGS__(__VA_ARGS__), _FS_SUN_SPAWN_31_1_(param, FS_SUN_EMPTY))

#define _FS_SUN_ARGC_PRESET_ 31, 30,            \
        29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
        19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
        9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define FS_SUN_ARGC(...) FS_SUN_GET32TH_START_FROM_2ND(FS_SUN_COMMA__VA_ARGS__(__VA_ARGS__), _FS_SUN_ARGC_PRESET_)

static_assert(FS_SUN_ARGC()     == 0, "FS_SUN_ARGC error 0");
static_assert(FS_SUN_ARGC(a)    == 1, "FS_SUN_ARGC error 1");
static_assert(FS_SUN_ARGC(a, a) == 2, "FS_SUN_ARGC error 2");
static_assert(FS_SUN_ARGC(a, a, a, a, a, a, a, a, a, a,
                          a, a, a, a, a, a, a, a, a, a,
                          a, a, a, a, a, a, a, a, a, a,
                          a)     == 31, "FS_SUN_ARGC error 31");

/**********************************/

#ifdef _MSC_VER
#define FS_SUN_FUNC_NAME __FUNCSIG__
#elif defined (__GNUC__)
#define FS_SUN_FUNC_NAME __PRETTY_FUNCTION__
#endif

#ifdef NDEBUG
#define FS_SUN_ASSERT(condition) 
#else
#define FS_SUN_ASSERT(condition)                                        \
    {                                                                   \
        if(!(condition))                                                \
        {                                                               \
            std::string msg;                                            \
            msg += "**************** FS_SUN_ASSERT FAILED ****************\n"; \
            msg += "**************** FS_SUN_ASSERT FAILED ****************\n"; \
            msg += "@CONDITION: " #condition "\n";                      \
            msg = msg + "@LINE: "  + std::to_string(__LINE__) + "\n";   \
            msg = msg + "@FILE: " + __FILE__ + "\n";                    \
            msg = msg + "@FUNCTION: " + FS_SUN_FUNC_NAME + "\n";        \
            msg += "**************** FS_SUN_ASSERT FAILED ****************\n"; \
            fs::Sun::cout("FS_SUN_ASSERT", msg, fs::Sun::logger::S_FATAL); \
            assert(false);                                              \
        }                                                               \
    }
#endif

FS_SUN_NS_BEGIN

/**************/
/** singleton */
/**************/
template <typename T>
class singleton
{
public:
    static T & Instance()
    {
        static T instance;
        return instance;
    }
    singleton(const singleton &) = delete;
    singleton & operator=(const singleton &) = delete;
protected:
    singleton() = default;
    ~singleton() = default;
};

template <typename T>
T & GetSingleton()
{
    static T instance;
    return instance;
}

/*****************/
/** string utils */
/*****************/

template<typename To, typename From>
inline To* cast(From* from)
{
#ifdef NDEBUG
    return static_cast<To*>(from);
#else
    To* to = dynamic_cast<To*>(from);
    assert(to != nullptr);
    return to;
#endif    
}

template<typename To, typename From>
inline To& cast(From && from)   /**TODO */
{
#ifdef NDEBUG
    return static_cast<To&&>(from);
#else
    return dynamic_cast<To&&>(from);
#endif    
}

template <typename T>
struct less_ptr
{
    bool operator()(const T* l, const T* r) const
    {
        return (*l) < (*r);
    }
};

/** index sequence */
template <std::size_t ...>
struct index_sequence
{};
template <std::size_t n, std::size_t ... s>
struct make_index_sequence : make_index_sequence<n-1, n-1, s...>
{};

template <std::size_t ... s>
struct make_index_sequence<0, s...>
{
    using type = index_sequence<s...>;
};
template <typename ... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;

template <typename func_t, typename tuple_t, std::size_t ... idx>
typename std::remove_reference<func_t>::type::result_type
_apply(func_t && f, tuple_t && t, index_sequence<idx...>)
{
    return std::forward<func_t>(f)(std::get<idx>(std::forward<tuple_t>(t))...);
}
template <typename func_t, typename tuple_t>
typename std::remove_reference<func_t>::type::result_type
apply(func_t && f, tuple_t && t)
{
    return _apply(std::forward<func_t>(f), std::forward<tuple_t>(t),
                  typename make_index_sequence<
                  std::tuple_size<typename std::remove_reference<tuple_t>::
                  type>::value>::type{});
}

template <typename func_t, typename tuple_t>
void apply2promise(func_t && f, tuple_t && t,
                   std::promise<typename
                   std::enable_if<std::is_void<
                   typename std::remove_reference<
                   func_t>::type::result_type>::value, typename std::remove_reference<
                   func_t>::type::result_type>::type> & promise)
{
    apply(std::forward<func_t>(f), std::forward<tuple_t>(t));
    promise.set_value();
}

template <typename func_t, typename tuple_t>
void apply2promise(func_t && f, tuple_t && t,
                   std::promise<typename
                   std::enable_if<! (std::is_void<typename std::remove_reference<
                                     func_t>::type::result_type>::value),
                   typename std::remove_reference<
                   func_t>::type::result_type>::type> & promise)
{
    promise.set_value(apply(std::forward<func_t>(f), std::forward<tuple_t>(t)));
}

FS_SUN_NS_END
