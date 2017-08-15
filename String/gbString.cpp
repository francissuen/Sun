#include "gbString.h"

void gbString::operator=(const char* str)
{
  _data = str;
}

bool gbString::operator==(const char* str)
{
  return _data.compare(str)==0?true:false;
}

bool gbString::operator==(const gbString& str)
{
  return _data.compare((const char*)str)==0?true:false;
}

bool gbString::operator==(const char* str)const
{
  return (*const_cast<gbString*>(this)) == str;
}

bool gbString::operator==(const gbString& str)const
{
  return (*const_cast<gbString*>(this)) == str;
}

gbString gbString::operator+(const char* str)
{

  return  (_data + str).c_str();
}

gbString gbString::operator+(const unsigned int val)
{
  char szVal[_gb_SMAX_BUFFER_SIZE] = {'\0'};
  sprintf(szVal, "%d", val);
  
  return (_data + szVal).c_str();
}

gbString gbString::operator+(const int val)
{
  char szVal[_gb_SMAX_BUFFER_SIZE] = {'\0'};
  sprintf(szVal, "%d", val);
  
  return (_data + szVal).c_str();
}

gbString gbString::operator+(const float val)
{
  char szVal[_gb_SMAX_BUFFER_SIZE] = {'\0'};
  sprintf(szVal, "%f", val);
  
  return (_data + szVal).c_str();
}
void gbString::ExtractBlocks(const std::vector<std::string>& pairDelimiters, std::map<const std::string, std::string>& blocks)
{
	if (pairDelimiters.size() % 2 != 0)
	{
		return;
	}

	size_t curPos = 0;
	for (std::vector<std::string>::const_iterator i = pairDelimiters.begin(); i != pairDelimiters.end(); i++)
	{
		size_t startPos = _data.find(*i, curPos);
		size_t dlmtrLen = i->size();
		i++;
		if (startPos != std::string::npos)
		{
			size_t blockStartPos = startPos + dlmtrLen;
			//curPos = blockStartPos;

			size_t endPos = _data.find(*i, blockStartPos);
			if (endPos != std::string::npos)
			{
				blocks.insert(std::pair<const std::string, std::string>(*i, _data.substr(blockStartPos, endPos - blockStartPos)));
				//blocks.push_back(_data.substr(blockStartPos, endPos - blockStartPos));
				//curPos = endPos + i->size();
			}
		}
	}
}
void gbString::Split(const char* delimeter, std::vector<gbString>& strs)
{
  size_t curPos = 0;
  while(true)
    {
      size_t pos = _data.find(delimeter, curPos);
      if(pos != std::string::npos)
	{
	  strs.push_back(_data.substr(curPos, pos).c_str());
	  curPos += (pos + strlen(delimeter));
	}
      else
	{
	  strs.push_back(_data.substr(curPos).c_str());
	  break;
	}
    }
}

void gbString::Replace(const char* dest, const char* src )
{
  size_t pos = _data.find(dest);
  if(pos != std::string::npos)
    {
      unsigned int len = strlen(dest);
      _data.replace(pos, len, src);
    }
}
gbString gbString::SubStrAtLastOf_L(const char val, const bool exclude)
{
  size_t pos =  _data.find_last_of(val);
  if(pos == std::string::npos)
    return _data.c_str();
  else
    return _data.substr(0, exclude?pos:pos + 1).c_str();
}

gbString gbString::SubStrAtLastOf_R(const char val, const bool exclude)
{
  size_t pos =  _data.find_last_of(val);
  if(pos == std::string::npos)
    return _data.c_str();
  else
    return _data.substr(exclude ? pos + 1 : pos).c_str();
}
