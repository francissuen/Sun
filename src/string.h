/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include "common.h"
#include "config.h"
#include <string>
#include <type_traits>
#include <map>
#include <vector>

#include "ns.h"

FS_SUN_NS_BEGIN

/**
 * \brief A extended std::string
 */
FS_SUN_CLASS string: public std::string /**TOD wstring */
{
public:
    inline string(const std::string & str):
    std::string(str)
    {}
    inline string(std::string && str):
    std::string(std::move(str))
    {}

    /** operator +=, + for numeric type */
    template<typename number_t>
    typename std::enable_if<std::is_arithmetic<number_t>::value,
                            string &>::type
    operator+=(number_t num)
    {
        this->operator+=(std::to_string(num));
        return *this;
    }

    template<typename string_t, typename number_t>
    friend typename std::enable_if<std::is_arithmetic<number_t>::value,
                                   std::decay<string_t>>::type
    operator+(string_t && str, number_t num)
    {
        return std::forward<string_t>(str) + std::to_string(num);
    }

    template<typename string_t, typename number_t>
    friend typename std::enable_if<std::is_arithmetic<number_t>::value,
                                   std::decay<string_t>>::type
    operator+(number_t num, string_t && str)
    {
        return std::forward<string_t>(str) + std::to_string(num);
    }

    
/**
 * \brief Extract string blocks which are all between of each \b pairedDelimiters.
 * 
 * \return String blocks are always returned and mapped by key std::pair<>::first of
 * pairedDelimiter.
 */
    std::map<const std::string, std::string> extract_blocks(
        const std::vector<std::pair<std::string, std::string>>& pairedDelimiters)const;
    std::vector<string> split(const char* delimiter)const;
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
};



FS_SUN_NS_END

namespace std
{
    template <>
    struct hash<fs::Sun::string>
    {
	std::size_t operator()(const fs::Sun::string& str) const noexcept
        {
            return std::hash<std::string>()(str);
        }
    };

    template <>
    struct hash<const fs::Sun::string>
    {
	std::size_t operator()(const fs::Sun::string& str) const noexcept
        {
            return std::hash<std::string>()(str);
        }
    };
}
