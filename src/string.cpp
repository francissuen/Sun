/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#include "string.h"

using fs::Sun::string;

std::map<const std::string, std::string> string::extract_blocks(
    const std::vector<std::pair<std::string, std::string>>& pairedDelimiters)const
{
    std::map<const std::string, std::string> ret;

    for(const auto pd : pairedDelimiters)
    {
        const std::string & first = pd.first;
	const size_t startPos = find(first, 0);
	const size_t dlmtrLen = first.size();

	if (startPos != std::string::npos)
	{
	    const size_t blockStartPos = startPos + dlmtrLen;

	    const size_t endPos = find(pd.second, blockStartPos);
	    if (endPos != std::string::npos)
	    {
		ret.insert(std::make_pair(pd.first, substr(blockStartPos, endPos - blockStartPos)));
	    }
	}
    }

    return ret;
}

std::vector<string> string::split(const char* delimeter)const
{
    FS_SUN_ASSERT(delimeter != nullptr);
    std::vector<string> ret;
    size_t curPos = 0;
    const size_t dlmtrLen = strlen(delimeter);
    while(true)
    {
	const size_t pos = find(delimeter, curPos);
	if(pos != std::string::npos)
	{
	    ret.push_back(substr(curPos, pos));
	    curPos += (pos + dlmtrLen);
	}
	else
	{
	    ret.push_back(substr(curPos));
	    break;
	}
    }
    return ret;
}

string string::file_extension() const
{
    const size_t pos = find_last_of('.');
    if(pos == std::string::npos)
        return string("");
    else
        return string(substr(pos + 1));
}
