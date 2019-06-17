/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include "config.h"
#include <string>
#include <type_traits>
#include <vector>

#include "ns.h"

FS_SUN_NS_BEGIN

namespace string
{
/**
 * \brief Extract string blocks which are all between of each \b pairedDelimiters.
 * 
 * \return String blocks are always returned and mapped by key std::pair<>::first of
 * pairedDelimiter.
 */
    std::vector<std::string> ExtractBlocks(
        const std::string & src,
        const std::vector<std::pair<std::string, std::string>>& pairedDelimiters);
    
    std::vector<std::string> Split(const std::string & src, const char* delimiter);
    
    std::string FileExtension(const std::string & src);

    template<typename string_0, typename string_1>
    std::string Concat(string_0 && str0, string_1 && str1)
    {
        return str0 + str1;
    }
    template<typename string_0, typename string_1, typename string_2, typename ... string_n>
    std::string Concat(string_0 && str0, string_1 && str1, string_2 && str2, string_n ... strn)
    {
        return Concat(concat(std::forward<string_0>(str0), std::forward<string_1>(str1)),
                      str2, strn ...);
    }

    template<typename string_0, typename string_1>
    std::string ConcatWithDelimiter(const char* delimiter, string_0 && str0, string_1 && str1)
    {
        return str0 + delimiter + str1;
    }
    
    template<typename string_0, typename string_1, typename string_2, typename ... string_n>
    std::string ConcatWithDelimiter(const char* delimiter, string_0 && str0, string_1 && str1,
                                    string_2 && str2, string_n ... strn)
    {
        return Concat(delimiter, Concat(delimiter, std::forward<string_0>(str0),
                                        std::forward<string_1>(str1)),
                      str2, strn ...);
    }
}

FS_SUN_NS_END

namespace std
{
    std::string to_string(const bool value);
}
