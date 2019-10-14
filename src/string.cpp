/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#include "string.h"
#include "logger.h"
#include "debug.h"
#include <cstring>

FS_SUN_NS_BEGIN
namespace string
{
    std::vector<std::string> ExtractBlocks(
        const std::string & src,
        const std::vector<std::pair<std::string, std::string>>& bounding_delimiters)
    {
        std::vector<std::string> ret;

        for(const auto bd : bounding_delimiters)
        {
            const std::string & first = bd.first;
            const std::size_t begin_pos = src.find(first, 0);
            const std::size_t delrs_len = first.size();

            if (begin_pos != std::string::npos)
            {
                const std::size_t block_begin_pos = begin_pos + delrs_len;

                const std::size_t end_pos = src.find(bd.second, block_begin_pos);
                if (end_pos != std::string::npos)
                {
                    ret.push_back(src.substr(block_begin_pos, end_pos - block_begin_pos));
                }
            }
        }

        return ret;
    }

    std::vector<std::string> Split(const std::string & src, const char* delimeter)
    {
        FS_SUN_ASSERT(delimeter != nullptr);
        std::vector<std::string> ret;
        std::size_t cur_pos = 0;
        const std::size_t delrs_len = std::strlen(delimeter);
        while(true)
        {
            const std::size_t pos = src.find(delimeter, cur_pos);
            if(pos != std::string::npos)
            {
                ret.push_back(src.substr(cur_pos, pos));
                cur_pos += (pos + delrs_len);
            }
            else
            {
                ret.push_back(src.substr(cur_pos));
                break;
            }
        }
        return ret;
    }

    std::string FileExtension(const std::string & src)
    {
        const size_t pos = src.find_last_of('.');
        if(pos == std::string::npos)
            return std::string("");
        else
            return src.substr(pos + 1);
    }

}

std::string to_string(const bool value)
{
    return value? "true" : "false";
}

FS_SUN_NS_END

