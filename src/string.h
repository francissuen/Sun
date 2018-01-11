#pragma once
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include "common.h"
#include "config.h"
#include <iostream>
#include <cassert>
#include "ns.h"


#define _GB_UTILS_STRING_MAX_BUFFER_SIZE 64

GB_UTILS_NS_BEGIN

/*
 * a wrapper for std::string
 */
GB_UTILS_CLASS string
{
public:

    // ctor
    inline string() {}
    inline string(const char* str) :_data(str) {}

/*
 *if std::string can conver to string implicit,
 *then operator+ may have ambiguous issue with std::string::operator+
 */
    explicit string(std::string && str) :
	_data(std::move(str))
    {}
    explicit string(const std::string& str) :
	_data(str)
    {}

    // copy ctor
    template<typename string_t>
	string(string_t && other) :
	_data(std::forward<string_t>(other._data))
    {}

    // assigment
    inline void operator=(const char* szStr)
	{
	    assert(szStr != nullptr);
	    _data = szStr;
	}
    inline void operator=(const std::string & str)
	{
	    _data = str;
	}
    inline void operator=(std::string && str)
	{
	    _data = std::move(str);
	}
    template<typename string_t>
	void operator=(string_t && other)
	{
	    _data = std::forward<string_t>(other._data);
	}

    // conversion
    inline operator const char*()const { return _data.c_str(); }
    inline explicit operator const std::string&()const { return _data; }

    // comparision
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

    // operator +=, +
    // string
    inline void operator +=(const string & other)
    {
	_data += other._data;
    }
    template<typename string_t>
	string operator+(string_t && other)const &
    {
	return string(_data + std::forward<string_t>(other._data));
    }
    template<typename string_t>
	string operator+(string_t && other) &&
    {
	return string(_data + std::forward<string_t>(other._data));
    }

    // std::string
    inline void operator +=(const std::string& str)
    {
	_data += str;
    }
    template<typename string_t, typename std_string>
	friend string operator + (string_t && str, std_string && ot)
    {
	return string(std::forward<string_t>(str._data) + std::forward<std_string>(ot));
    }
    template <typename string_t, typename std_string>
	friend string operator + (std_string && ot, string_t && str)
    {
	return string(std::forward<std_string>(ot) + std::forward<string_t>(str._data));
    }

    // const char*
    inline void string::operator+=(const char* szStr)
    {
	GB_ASSERT(szStr != nullptr);
	this->_data += szStr;
    }
    template<typename string_t>
	friend string operator+(string_t&& str, const char* szStr)
    {
	GB_ASSERT(szStr != nullptr);
	return string(std::forward<string_t>(str._data) + szStr);
    }
    template<typename string_t>
	friend string operator+(const char* szStr, string_t && str)
    {
	GB_ASSERT(szStr != nullptr);
	return string(szStr + std::forward<string_t>(str._data));
    }

    // misc type
#define _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(type, fmt)		\
    inline void operator+=(type ot)					\
    {									\
	char szVal[_GB_UTILS_STRING_MAX_BUFFER_SIZE] = {'\0'};		\
	sprintf(szVal, fmt, ot);					\
	this->_data += szVal;						\
    }									\
    template<typename string_t>						\
	friend string operator + (string_t && str, type ot)	\
    {									\
	char szVal[_GB_UTILS_STRING_MAX_BUFFER_SIZE] = {'\0'};		\
	sprintf(szVal, fmt, ot);					\
	return string(std::forward<string_t>(str._data) + szVal);	\
    }									\
    template<typename string_t>						\
	friend string operator + (type ot, string_t && str)		\
    {									\
	char szVal[_GB_UTILS_STRING_MAX_BUFFER_SIZE] = {'\0'};		\
	sprintf(szVal, fmt, ot);					\
	return string(szVal + std::forward<string_t>(str._data));	\
    }		

    _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(const char, "%c");
    _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(const int, "%d");
    _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(const unsigned int, "%u");
    _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(const float, "%f");
    _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(const long, "%ld");
    _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(const unsigned long, "%lu");

    // misc
    inline size_t length()const { return _data.length(); }

    //extract block with back delimiters as map's key
    std::map<const std::string, std::string> extract_blocks(const std::vector<std::string>& pairDelimiters)const;
    std::vector<string> split(const char* delimiter)const;
    void replace(const char* old_str, const char* new_str);
    string substr_at_l_lastof(const char val, const bool exclude = true);
    string substr_at_r_lastof(const char val, const bool exclude = true);

private:
    std::string _data;

};

GB_UTILS_NS_END

namespace std
{
    template <>
    struct hash<gb::utils::string>
    {
	std::size_t operator()(gb::utils::string& str) const noexcept
	    {
		return std::hash<std::string>()((std::string)str);
	    }
    };

    template <>
    struct hash<const gb::utils::string>
    {
	std::size_t operator()(const gb::utils::string& str) const noexcept
	    {
		return std::hash< std::string>()((const std::string)str);
	    }
    };
}
