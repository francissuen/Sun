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

    template <typename T> struct is_string : std::false_type {};

    // ctor
    inline string() {}
    inline string(const char* str) :_data(str) { GB_ASSERT(str != nullptr); }

    /*
     *if std::string can conver to string implicit,
     *then operator+ may have ambiguous issue with std::string::operator+
     */
    explicit string(const std::string & str):
	_data(str)
    {}
    explicit string(std::string && str):
	_data(std::move(str))
    {}
    
    // copy ctor
    string(const string & str):
	_data(str._data)
    {}

    string(string && str):
	_data(std::move(str._data))
    {}
    
    // copy assignment operator
    inline string & operator=(const string & other)
	{
	    _data = other._data;
	    return *this;
	}
    // move assignment operator
    inline string & operator=(string && other)
	{
	    _data = std::move(other._data);
	    return *this;
	}

    // assignment operator overloading for other types
    inline string & operator=(const char* szStr)
	{
	    GB_ASSERT(szStr != nullptr);
	    _data = szStr;
	    return *this;
	}
    template<typename std_string>
	typename std::enable_if<gb::is_std_string<typename gb::rm_cv_ref<std_string>::type>::value, string &>
	::type operator=(std_string && str)
	{
	    _data = std::forward<std_string>(str);
	    return *this;
	}

    // conversion
    inline operator const char*()const { return _data.c_str(); }

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

    // operator +=
    // string
    template <typename string_t>
	typename std::enable_if<is_string<typename gb::rm_cv_ref<string_t>::type>::value, void>
	::type operator += (string_t && other)
    {
	_data += other._data;
    }
    // std::string 
    template <typename std_string>
	typename std::enable_if<gb::is_std_string<typename gb::rm_cv_ref<std_string>>::value, void>
	::type operator += (std_string && other)
    {
	_data += std::forward<std_string>(other);    
    }
    // const char*
    void operator +=(const char* szStr)
    {
	GB_ASSERT(szStr != nullptr);
	_data += szStr;
    }
    
    // operator +
    // string
    template <typename string_t_l, typename string_t_r>
	friend typename std::enable_if<is_string<typename gb::rm_cv_ref<string_t_l>::type>::value &&
				       is_string<typename gb::rm_cv_ref<string_t_r>::type>::value, string>
	::type operator + (string_t_l && l, string_t_r && r)
    {
	return string(std::forward<string_t_l>(l)._data + std::forward<string_t_r>(r)._data);
    }

    // std::string
    template<typename string_t, typename std_string>
	friend typename std::enable_if<gb::is_std_string<typename gb::rm_cv_ref<std_string>::type>::value, string>
	::type operator + (string_t && l, std_string && r)
    {
	return string(std::forward<string_t>(l)._data + std::forward<std_string>(r));
    }
    template <typename string_t, typename std_string>
	friend typename std::enable_if<gb::is_std_string<typename gb::rm_cv_ref<std_string>::type>::value, string>
	::type operator + (std_string && l, string_t && r)
    {
	return string(std::forward<std_string>(l) + std::forward<string_t>(r)._data);
    }
    // const char*
    template<typename string_t>
	friend typename std::enable_if<is_string<typename gb::rm_cv_ref<string_t>::type>::value, string>
	::type operator + (string_t && l, const char* r)
    {
	return string(std::forward<string_t>(l)._data + r);
    }
    template<typename string_t>
	friend typename std::enable_if<is_string<typename gb::rm_cv_ref<string_t>::type>::value, string>
	::type operator + (const char* l, string_t && r)
    {
	return string(l + std::forward<string_t>(r)._data);
    }

    // misc type
#define _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(type, fmt)	\
    inline void operator+=(type ot)				\
    {								\
	char szVal[_GB_UTILS_STRING_MAX_BUFFER_SIZE] = {'\0'};	\
	sprintf(szVal, fmt, ot);				\
	this->_data += szVal;					\
    }								\
    template<typename string_t>					\
	friend string operator + (string_t && l, type r)	\
    {								\
	char szVal[_GB_UTILS_STRING_MAX_BUFFER_SIZE] = {'\0'};	\
	sprintf(szVal, fmt, r);					\
	return string(std::forward<string_t>(l)._data + szVal);	\
    }								\
    template<typename string_t>					\
	friend string operator + (type l, string_t && r)	\
    {								\
	char szVal[_GB_UTILS_STRING_MAX_BUFFER_SIZE] = {'\0'};	\
	sprintf(szVal, fmt, l);					\
	return string(szVal + std::forward<string_t>(r)._data);	\
    }		

    _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(const char, "%c");
    _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(const signed char, "%hhd");
    _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(const unsigned char, "%hhu");
    _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(const int, "%d");
    _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(const unsigned int, "%u");
    _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(const float, "%f");
    _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(const long, "%ld");
    _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE_(const unsigned long, "%lu");

    // misc
    inline size_t length()const { return _data.length(); }
    inline const std::string & GetStdString()const { return _data; }
    inline std::string& GetStdString() { return _data; }
    bool operator < (const string& other) const;

    //extract block with back delimiters as map's key
    std::map<const std::string, std::string> extract_blocks(const std::vector<std::string>& pairDelimiters)const;
    std::vector<string> split(const char* delimiter)const;
    void replace(const char* old_str, const char* new_str);
    string substr_at_l_lastof(const char val, const bool exclude = true);
    string substr_at_r_lastof(const char val, const bool exclude = true);

private:
    std::string _data;

};

template <> struct string::is_string<string> : std::true_type {};

GB_UTILS_NS_END

namespace std
{
    template <>
    struct hash<gb::utils::string>
    {
	std::size_t operator()(const gb::utils::string& str) const noexcept
	    {
		return std::hash<std::string>()(str.GetStdString());
	    }
    };

    template <>
    struct hash<const gb::utils::string>
    {
	std::size_t operator()(const gb::utils::string& str) const noexcept
	    {
		return std::hash<std::string>()(str.GetStdString());
	    }
    };
}
