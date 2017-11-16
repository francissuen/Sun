#pragma once
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include "common.h"
#include "../gbUtilsConfig.h"

#include <iostream>

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

	  /*
	   *@brief if std::string can conver to string implicit,
	   *then any member function which has same type arguments with std::string
	   *may be encounter ambiguous issue
	   */
	  inline explicit string(const std::string& str):
	      _data(str)
	  {}
	  inline string(const string& other):
	      _data(other._data)
	  {}
	  inline string(const string&& other):
	      _data(std::move(other._data))
	  {}

	  inline ~string()
	  {}

	  inline operator const char*()const { return _data.c_str(); }
	  
	  inline explicit operator const std::string&()const { return _data; }
	  
	  void operator=(const char* str);
	  inline void operator=(const string& other)
	      {
		  _data = other._data;
	      }
	  
	  bool operator==(const char* str)const;
	  bool operator==(const std::string& str)const;

	  //const version
	  friend string operator+(const string& str, const char* szStr);
	  friend string operator+(const string& str, const char val);

	  template<typename intoruint>
	  friend string operator+(const string& str, const intoruint val);

	  friend string operator+(const string& str, const float val);

	  //non-const rvalue ref version
	  friend string operator+(string&& str, const char* szStr);
	  friend string operator+(string&& str, const char val);
	  template<typename intoruint>
	  friend string operator+(string&& str, const intoruint val);

	  friend string operator+(string&& str, const float val);
	      
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
