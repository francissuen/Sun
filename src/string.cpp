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
            const size_t startPos = src.find(first, 0);
            const size_t dlmtrLen = first.size();

            if (startPos != std::string::npos)
            {
                const size_t blockStartPos = startPos + dlmtrLen;

                const size_t endPos = src.find(bd.second, blockStartPos);
                if (endPos != std::string::npos)
                {
                    ret.push_back(src.substr(blockStartPos, endPos - blockStartPos));
                }
            }
        }

        return ret;
    }

    std::vector<std::string> Split(const std::string & src, const char* delimeter)
    {
        FS_SUN_ASSERT(delimeter != nullptr);
        std::vector<std::string> ret;
        size_t curPos = 0;
        const size_t dlmtrLen = std::strlen(delimeter);
        while(true)
        {
            const size_t pos = src.find(delimeter, curPos);
            if(pos != std::string::npos)
            {
                ret.push_back(src.substr(curPos, pos));
                curPos += (pos + dlmtrLen);
            }
            else
            {
                ret.push_back(src.substr(curPos));
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

