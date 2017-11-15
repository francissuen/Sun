#pragma once
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include "common.h"
#include "gbUtilsConfig.h"


namespace gb
{
  namespace utils
  {
      /*
       * a wrapper for std::string
       */
      gbUtilsEXPORT_CLASS string
      {
      public:
	  inline string(){}
	  inline string(const char* str):_data(str){}
	  inline string(const string& other):
	      _data(other._data)
	  {}
	  inline string(const string&& other):
	      _data(std::move(other._data))
	  {}
  
	  inline operator const char* ()const{return _data.c_str();}
	  inline operator std::string& (){return _data; }
  
	  void operator=(const char* str);
	  inline void operator=(const string& other)
	      {
		  _data = other._data;
	      }
	  
	  bool operator==(const char* str);
	  bool operator==(const char* str)const;

	  bool operator==(const string& str);
	  bool operator==(const string& str)const;
  
	  string operator+(const char* str);
	  string operator+(const char val);
	  string operator+(const unsigned int val);
	  string operator+(const int val);
	  string operator+(const float val);

	  //extract block with back delimiters as map's key
	  std::map<const std::string, std::string> extract_blocks(const std::vector<std::string>& pairDelimiters)const;
	  std::vector<string> split(const char* delimiter)const;
	  void replace(const char* old_str, const char* new_str);
	  string substr_at_l_lastof(const char val, const bool exclude = true);
	  string substr_at_r_lastof(const char val, const bool exclude = true);

      private:
	  std::string _data;
	  
      };
  };
};
