#pragma once
#include <string>
#include <vector>
#include <map>
#include "gbCommon.h"
#include "gbUtilsConfig.h"
#define _gb_SMAX_BUFFER_SIZE 64
gbUtilsEXPORT_CLASS gbString
{
public:
  inline gbString(){}
  inline gbString(const char* str):_data(str){}
  inline operator const char* ()const{return _data.c_str();}
  
  void operator=(const char* str);

  bool operator==(const char* str);
  bool operator==(const char* str)const;

  bool operator==(const gbString& str);
  bool operator==(const gbString& str)const;
  
  gbString operator+(const char* str);
  gbString operator+(const unsigned int val);
  gbString operator+(const int val);
  gbString operator+(const float val);

  //extract block with back delimiters as map's key
  void ExtractBlocks(const std::vector<std::string>& pairDelimiters, std::map<const std::string, std::string>& blocks);
  void Split(const char* delimiter, std::vector<gbString>& strs);
  void Replace(const char* dest, const char* src );
  gbString SubStrAtLastOf_L(const char val, const bool exclude = true);
  gbString SubStrAtLastOf_R(const char val, const bool exclude = true);

private:
  std::string _data;
};
