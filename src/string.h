/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include "common.h"
#include "config.h"
#include <iostream>
#include <cassert>
#include <cinttypes>

#include "ns.h"


#define _FS_SUN_STRING_MAX_BUFFER_SIZE 64

FS_SUN_NS_BEGIN

template <typename T> struct _is_string : std::false_type {};
class string;
template <> struct _is_string<string> : std::true_type {};
template <typename T>
struct is_string: _is_string<typename std::decay<T>::type> {};

/**
 * \brief A wrapper for std::string, but with more useful features.
 */
FS_SUN_CLASS string
{
public:
// ctor
inline string() {}
inline string(const char* str) :_data(str) { FS_SUN_ASSERT(str != nullptr); }

inline string(const string & str):
_data(str._data)
{}

inline string(string && str):
_data(std::move(str._data))
{}

/*
 * if std::string can be convered to string implicit,
 * then operator+ may have ambiguous issue with std::string::operator+
 */
template <typename std_string,
typename std::enable_if<is_std_string<std_string>::value, int>::type = 0>
explicit string(std_string && str):
_data(std::forward<std_string>(str))
{}

template <typename string_t>
typename std::enable_if<is_string<string_t>::value, string &>::type
operator=(string_t && other)
{
    _data = std::forward<string_t>(other)._data;
    return *this;
}

inline string & operator=(const char* szStr)
{
    FS_SUN_ASSERT(szStr != nullptr);
    _data = szStr;
    return *this;
}
template<typename std_string>
typename std::enable_if<is_std_string<std_string>::value, string &>::type
operator=(std_string && str)
{
    _data = std::forward<std_string>(str);
    return *this;
}


inline operator const char*()const { return _data.c_str(); }

bool operator==(const char* szStr)const;
bool operator==(const std::string& str)const;
inline bool operator!=(const char* szStr)const
{
    return !operator==(szStr);
}
inline bool operator!=(const std::string& str)const
{
    return !operator==(str);
}

// operator +=
// string
template <typename string_t>
typename std::enable_if<is_string<string_t>::value, void>::type
operator += (string_t && other)
{
    _data += std::forward<string_t>(other)._data;
}
// std::string 
template <typename std_string>
typename std::enable_if<is_std_string<std_string>::value, void>::type
operator += (std_string && other)
{
    _data += std::forward<std_string>(other);    
}
// const char*
void operator +=(const char* szStr)
{
    FS_SUN_ASSERT(szStr != nullptr);
    _data += szStr;
}
    
// operator +
// string
template <typename string_t_l, typename string_t_r>
friend typename std::enable_if<is_string<string_t_l>::value &&
is_string<string_t_r>::value, string>::type
operator + (string_t_l && l, string_t_r && r)
{
    return string(std::forward<string_t_l>(l)._data + std::forward<string_t_r>(r)._data);
}

// std::string
template<typename string_t, typename std_string>
friend typename std::enable_if<is_string<string_t>::value &&
is_std_string<std_string>::value, string>::type
operator + (string_t && l, std_string && r)
{
    return string(std::forward<string_t>(l)._data + std::forward<std_string>(r));
}
template <typename string_t, typename std_string>
friend typename std::enable_if<is_string<string_t>::value &&
is_std_string<std_string>::value, string>::type
operator + (std_string && l, string_t && r)
{
    return string(std::forward<std_string>(l) + std::forward<string_t>(r)._data);
}
// const char*
template<typename string_t>
friend typename std::enable_if<is_string<string_t>::value, string>::type
operator + (string_t && l, const char* r)
{
    return string(std::forward<string_t>(l)._data + r);
}
template<typename string_t>
friend typename std::enable_if<is_string<string_t>::value, string>::type
operator + (const char* l, string_t && r)
{
    return string(l + std::forward<string_t>(r)._data);
}

// misc type
#define _FS_SUN_STRING_OPERATOR_PLUS_DEFINE_(TYPE, FMT)                 \
    inline void operator+=(TYPE ot)					\
{									\
    char szVal[_FS_SUN_STRING_MAX_BUFFER_SIZE] = {'\0'};		\
    sprintf(szVal, FMT, ot);						\
    this->_data += szVal;						\
}									\
template<typename string_t>						\
friend typename std::enable_if<is_string<string_t>::value, string>::type \
operator + (string_t && l, TYPE r)                                      \
{									\
    char szVal[_FS_SUN_STRING_MAX_BUFFER_SIZE] = {'\0'};		\
    sprintf(szVal, FMT, r);						\
    return string(std::forward<string_t>(l)._data + szVal);		\
}									\
template<typename string_t>						\
friend typename std::enable_if<is_string<string_t>::value, string>::type \
operator + (TYPE l, string_t && r)                                      \
{									\
    char szVal[_FS_SUN_STRING_MAX_BUFFER_SIZE] = {'\0'};		\
    sprintf(szVal, FMT, l);						\
    return string(szVal + std::forward<string_t>(r)._data);		\
}		

_FS_SUN_STRING_OPERATOR_PLUS_DEFINE_(const char, "%c");

//integer
#define _FS_SUN_STRING_OPERATOR_PLUS_DEFINE_INTEGER(SIZE)		\
    _FS_SUN_STRING_OPERATOR_PLUS_DEFINE_(const std::int##SIZE##_t, "%" PRId##SIZE) \
_FS_SUN_STRING_OPERATOR_PLUS_DEFINE_(const std::uint##SIZE##_t, "%" PRIu##SIZE) 

_FS_SUN_STRING_OPERATOR_PLUS_DEFINE_INTEGER(8);
_FS_SUN_STRING_OPERATOR_PLUS_DEFINE_INTEGER(16);
_FS_SUN_STRING_OPERATOR_PLUS_DEFINE_INTEGER(32);
_FS_SUN_STRING_OPERATOR_PLUS_DEFINE_INTEGER(64);

#ifdef __APPLE__
_FS_SUN_STRING_OPERATOR_PLUS_DEFINE_(const long, "%ld");
_FS_SUN_STRING_OPERATOR_PLUS_DEFINE_(const unsigned long, "%lu");
#endif
//_FS_SUN_STRING_OPERATOR_PLUS_DEFINE_(const int, "%d");
//_FS_SUN_STRING_OPERATOR_PLUS_DEFINE_(const unsigned int, "%u");
_FS_SUN_STRING_OPERATOR_PLUS_DEFINE_(const float, "%f");

/** dupicate with std::[u]int(32)|(64) above */
/** _FS_SUN_STRING_OPERATOR_PLUS_DEFINE_(const long, "%ld"); */
/** _FS_SUN_STRING_OPERATOR_PLUS_DEFINE_(const unsigned long, "%lu"); */

inline size_t length()const { return _data.length(); }
inline const std::string & GetStdString()const { return _data; }
inline const char * data() const { return _data.data(); }
inline std::string& get_std_string() { return _data; }
bool operator < (const string& other) const;

/**
 * \brief Extract string blocks which are all between of each \b pairedDelimiters.
 * 
 * \return String blocks are always returned and mapped by key std::pair<>::first of
 * pairedDelimiter.
 */
std::map<const std::string, std::string> extract_blocks(
    const std::vector<std::pair<std::string, std::string>>& pairedDelimiters)const;
std::vector<string> split(const char* delimiter)const;
void replace(const char* old_str, const char* new_str);
string substr_at_lhs_of_last(const char val, const bool exclude = true) const;
string substr_at_rhs_of_last(const char val, const bool exclude = true) const;
string file_extension() const;

template <typename T = void>
string concat() const
{
    return *this;
}

template <typename T>
string concat(T t) const
{
    return *this + t;
}

template <typename First_t, typename Second_t, typename ... Others_t>
string concat(First_t first, Second_t second, Others_t ... others) const
{
    return concat(first).concat(second, others...);
}

template <typename T = void>
string concat_with_delimiter(const char*) const
{
    return *this;
}

template <typename T>
string concat_with_delimiter(const char * delimiter, T t) const
{
    return *this + delimiter + t;
}

template <typename First_t, typename Second_t, typename ... Others_t>
string concat_with_delimiter(const char* delimiter,
                             First_t first,
                             Second_t second,
                             Others_t ... others)
{
    return concat_with_delimiter(delimiter, first).concat_with_delimiter(delimiter,
                                                                         second,
                                                                         others...);
}

private:
#ifdef _MSC_VER
#pragma warning(disable:4251)
#pragma warning(push)
#endif
std::string _data;
#ifdef _MSC_VER
#pragma warning(pop)
#endif

};



FS_SUN_NS_END

namespace std
{
    template <>
    struct hash<fs::Sun::string>
    {
	std::size_t operator()(const fs::Sun::string& str) const noexcept
        {
            return std::hash<std::string>()(str.GetStdString());
        }
    };

    template <>
    struct hash<const fs::Sun::string>
    {
	std::size_t operator()(const fs::Sun::string& str) const noexcept
        {
            return std::hash<std::string>()(str.GetStdString());
        }
    };
}
