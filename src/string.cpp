#include "string.h"
using gb::utils::string;

#define _GB_UTILS_STRING_MAX_BUFFER_SIZE 64

void string::operator=(const char* str)
{
    _data = str;
}

bool string::operator==(const char* str)
{
    return _data.compare(str)==0?true:false;
}

bool string::operator==(const string& str)
{
    return _data.compare((const char*)str)==0?true:false;
}

bool string::operator==(const char* str)const
{
    return (*const_cast<string*>(this)) == str;
}

bool string::operator==(const string& str)const
{
    return (*const_cast<string*>(this)) == str;
}

string string::operator+(const char* str)
{

    return  (_data + str).c_str();
}

string string::operator+(const char val)
{
    return (_data + val).c_str();
}
string string::operator+(const unsigned int val)
{
    char szVal[_GB_UTILS_STRING_MAX_BUFFER_SIZE] = {'\0'};
    sprintf(szVal, "%d", val);
  
    return (_data + szVal).c_str();
}

string string::operator+(const int val)
{
    char szVal[_GB_UTILS_STRING_MAX_BUFFER_SIZE] = {'\0'};
    sprintf(szVal, "%d", val);
  
    return (_data + szVal).c_str();
}

string string::operator+(const float val)
{
    char szVal[_GB_UTILS_STRING_MAX_BUFFER_SIZE] = {'\0'};
    sprintf(szVal, "%f", val);
  
    return (_data + szVal).c_str();
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
	size_t startPos = _data.find(*i, 0);
	size_t dlmtrLen = i->size();
	i++;
	if (startPos != std::string::npos)
	{
	    size_t blockStartPos = startPos + dlmtrLen;
	    //curPos = blockStartPos;

	    size_t endPos = _data.find(*i, blockStartPos);
	    if (endPos != std::string::npos)
	    {
		ret.insert(std::pair<const std::string, std::string>(*i, _data.substr(blockStartPos, endPos - blockStartPos)));
		//blocks.push_back(_data.substr(blockStartPos, endPos - blockStartPos));
		//curPos = endPos + i->size();
	    }
	}
    }
    return ret;
}

std::vector<string> string::split(const char* delimeter)const
{
    std::vector<string> ret;
    size_t curPos = 0;
    while(true)
    {
	size_t pos = _data.find(delimeter, curPos);
	if(pos != std::string::npos)
	{
	    ret.push_back(_data.substr(curPos, pos).c_str());
	    curPos += (pos + strlen(delimeter));
	}
	else
	{
	    ret.push_back(_data.substr(curPos).c_str());
	    break;
	}
    }
    return ret;
}

void string::replace(const char* old_str, const char* new_str)
{
    const size_t pos = _data.find(old_str);
    if(pos != std::string::npos)
    {
	const unsigned int len = strlen(old_str);
	_data.replace(pos, len, new_str);
    }
}
string string::substr_at_l_lastof(const char val, const bool exclude)
{
    size_t pos =  _data.find_last_of(val);
    if(pos == std::string::npos)
	return _data.c_str();
    else
	return _data.substr(0, exclude?pos:pos + 1).c_str();
}

string string::substr_at_r_lastof(const char val, const bool exclude)
{
    size_t pos =  _data.find_last_of(val);
    if(pos == std::string::npos)
	return _data.c_str();
    else
	return _data.substr(exclude ? pos + 1 : pos).c_str();
}
