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
