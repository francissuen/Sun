#include "string.h"
using gb::utils::string;

bool string::operator==(const char* szStr)const
{
    assert(szStr != nullptr);
    return _data.compare(szStr) == 0? true : false;
}
bool string::operator==(const std::string& str)const
{
    return _data.compare(str) == 0? true : false;
}

bool string::operator < (const string& other) const
{
    return _data < other._data;
}
std::map<const std::string, std::string> string::extract_blocks(const std::vector<std::string>& pairDelimiters)const
{
    std::map<const std::string, std::string> ret;
    if (pairDelimiters.size() % 2 != 0)
    {
	return ret;
    }

    for (std::vector<std::string>::const_iterator i = pairDelimiters.begin(); i != pairDelimiters.end(); i++)
    {
	const size_t startPos = _data.find(*i, 0);
	const size_t dlmtrLen = i->size();
	i++;
	if (startPos != std::string::npos)
	{
	    const size_t blockStartPos = startPos + dlmtrLen;

	    const size_t endPos = _data.find(*i, blockStartPos);
	    if (endPos != std::string::npos)
	    {
		ret.insert(std::pair<const std::string, std::string>(*i, _data.substr(blockStartPos, endPos - blockStartPos)));
	    }
	}
    }
    return ret;
}

std::vector<string> string::split(const char* delimeter)const
{
    GB_ASSERT(delimeter != nullptr);
    std::vector<string> ret;
    size_t curPos = 0;
    const size_t dlmtrLen = strlen(delimeter);
    while(true)
    {
	const size_t pos = _data.find(delimeter, curPos);
	if(pos != std::string::npos)
	{
	    ret.push_back(string(_data.substr(curPos, pos)));
	    curPos += (pos + dlmtrLen);
	}
	else
	{
	    ret.push_back(string(_data.substr(curPos)));
	    break;
	}
    }
    return ret;
}

void string::replace(const char* old_str, const char* new_str)
{
    assert(old_str != nullptr && new_str != nullptr);
    const size_t pos = _data.find(old_str);
    if(pos != std::string::npos)
    {
	const size_t len = strlen(old_str);
	_data.replace(pos, len, new_str);
    }
}
string string::substr_at_l_lastof(const char val, const bool exclude) const
{
    const size_t pos =  _data.find_last_of(val);
    if(pos == std::string::npos)
	return string(_data);
    else
	return string(_data.substr(0, exclude?pos:pos + 1));
}

string string::substr_at_r_lastof(const char val, const bool exclude) const
{
    const size_t pos =  _data.find_last_of(val);
    if(pos == std::string::npos)
	return string(_data);
    else
	return string(_data.substr(exclude ? pos + 1 : pos));
}
string string::file_extension() const
{
    return substr_at_r_lastof('.', true);
}
