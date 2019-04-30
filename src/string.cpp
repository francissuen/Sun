/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#include "string.h"
#include "logger.h"
using namespace fs::Sun::string;

std::unordered_map<const std::string, std::string> extract_blocks(
    const std::string & src,
    const std::vector<std::pair<std::string, std::string>>& pairedDelimiters)
{
    std::unordered_map<const std::string, std::string> ret;

    for(const auto pd : pairedDelimiters)
    {
        const std::string & first = pd.first;
	const size_t startPos = src.find(first, 0);
	const size_t dlmtrLen = first.size();

	if (startPos != std::string::npos)
	{
	    const size_t blockStartPos = startPos + dlmtrLen;

	    const size_t endPos = src.find(pd.second, blockStartPos);
	    if (endPos != std::string::npos)
	    {
		ret.insert(std::make_pair(pd.first,
                                          src.substr(blockStartPos, endPos - blockStartPos)));
	    }
	}
    }

    return ret;
}

std::vector<std::string> split(const std::string & src, const char* delimeter)
{
    FS_SUN_ASSERT(delimeter != nullptr);
    std::vector<std::string> ret;
    size_t curPos = 0;
    const size_t dlmtrLen = strlen(delimeter);
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

std::string file_extension(const std::string & src)
{
    const size_t pos = src.find_last_of('.');
    if(pos == std::string::npos)
        return std::string("");
    else
        return src.substr(pos + 1);
}
