#include "string.h"
using gb::utils::string;

#define _GB_UTILS_STRING_MAX_BUFFER_SIZE 64

bool string::operator==(const char* szStr)const
{
    assert(szStr != nullptr);
    return _data.compare(szStr) == 0? true : false;
}
bool string::operator==(const std::string& str)const
{
    return _data.compare(str) == 0? true : false;
}

string string::operator+(const string& other)const &
{
    return string(this->_data + other._data);
}

string string::operator+(string&& other)const &
{
    other._data.insert(0, this->_data);
    return std::move(other);
}

string string::operator+(const string& other)&&
{
    this->_data += other._data;
    return std::move(*this);
}

string string::operator+(string&& other)&&
{
    this->_data += other._data;
    return std::move(*this);
}

void string::operator+=(const char* szStr)
{
    assert(szStr != nullptr);
    this->_data += szStr;
}
string gb::utils::operator+(const string& str, const char* szStr)
{
    assert(szStr != nullptr);
    return string(str._data + szStr);
}
string gb::utils::operator+(const char* szStr, const string& str)
{
    assert(szStr != nullptr);
    return string(szStr + str._data);
}
string gb::utils::operator+(string&& str, const char* szStr)
{
    assert(szStr != nullptr);
    str._data += szStr;
    //copy elision condition not met, so using move ctor
    return std::move(str);
}
string gb::utils::operator+(const char* szStr, string&& str)
{
    assert(szStr != nullptr);
    str._data.insert(0, szStr);
    return std::move(str);
}

#define _GB_UTILS_STRING_OPERATOR_PLUS_DEFINE(type, fmt)		\
    /*+=*/								\
    void string::operator+=(type val)					\
    {									\
	char szVal[_GB_UTILS_STRING_MAX_BUFFER_SIZE] = {'\0'};		\
	sprintf(szVal, fmt, val);					\
	this->_data += szVal;						\
    }									\
    /*const string& + type*/						\
    string gb::utils::operator+(const string& str,type val)		\
    {									\
	char szVal[_GB_UTILS_STRING_MAX_BUFFER_SIZE] = {'\0'};		\
	sprintf(szVal, fmt, val);					\
	return string(str._data + szVal);				\
    }									\
    /*type + const string&*/						\
    string gb::utils::operator+(type val, const string& str)		\
    {									\
	char szVal[_GB_UTILS_STRING_MAX_BUFFER_SIZE] = {'\0'};		\
	sprintf(szVal, fmt, val);					\
	return string(szVal + str._data);				\
    }									\
    /*string&& + type*/							\
    string gb::utils::operator+(string&& str, type val)			\
    {									\
	char szVal[_GB_UTILS_STRING_MAX_BUFFER_SIZE] = {'\0'};		\
	sprintf(szVal, fmt, val);					\
	str._data += szVal;						\
	return std::move(str);						\
    }									\
    /*type + string&&*/							\
    string gb::utils::operator+(type val, string&& str)			\
    {									\
	char szVal[_GB_UTILS_STRING_MAX_BUFFER_SIZE] = {'\0'};		\
	sprintf(szVal, fmt, val);					\
	str._data.insert(0, szVal);					\
	return std::move(str);						\
    }

_GB_UTILS_STRING_OPERATOR_PLUS_DEFINE(const char, "%c");
_GB_UTILS_STRING_OPERATOR_PLUS_DEFINE(const int, "%d");
_GB_UTILS_STRING_OPERATOR_PLUS_DEFINE(const unsigned int, "%u");
_GB_UTILS_STRING_OPERATOR_PLUS_DEFINE(const float, "%f");
_GB_UTILS_STRING_OPERATOR_PLUS_DEFINE(const long, "%ld");
_GB_UTILS_STRING_OPERATOR_PLUS_DEFINE(const unsigned long, "%lu");

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
    assert(delimeter != nullptr);
    std::vector<string> ret;
    size_t curPos = 0;
    while(true)
    {
	size_t pos = _data.find(delimeter, curPos);
	if(pos != std::string::npos)
	{
	    ret.push_back(string(_data.substr(curPos, pos).c_str()));
	    curPos += (pos + strlen(delimeter));
	}
	else
	{
	    ret.push_back(string(_data.substr(curPos).c_str()));
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
	const unsigned int len = strlen(old_str);
	_data.replace(pos, len, new_str);
    }
}
string string::substr_at_l_lastof(const char val, const bool exclude)
{
    size_t pos =  _data.find_last_of(val);
    if(pos == std::string::npos)
	return string(_data);
    else
	return string(_data.substr(0, exclude?pos:pos + 1));
}

string string::substr_at_r_lastof(const char val, const bool exclude)
{
    size_t pos =  _data.find_last_of(val);
    if(pos == std::string::npos)
	return string(_data);
    else
	return string(_data.substr(exclude ? pos + 1 : pos));
}
