#pragma once
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include "common.h"

#include "gbUtilsConfig.h"

#include <iostream>
#include <cassert>
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
	   *if std::string can conver to string implicit,
	   *then operator+ may have ambiguous issue with std::string::operator+
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
	  
	  inline void operator=(const char* szStr)
	      {
		  assert(szStr != nullptr);
		  _data = szStr;
	      }
	  inline void operator=(const string& other)
	      {
		  _data = other._data;
	      }
	  
	  bool operator==(const char* szStr)const;
	  bool operator==(const std::string& str)const;

	  //lvalue version
	  string operator+(const string& other)const &;
	  string operator+(string&& other)const &;

	  //rvalue version
	  string operator+(const string& other)&&;
	  string operator+(string&& other)&&;
	      
	  //const version
	  GB_FRIEND_BINARY_OPERATOR_DECLAR(string, +, const string&, const char*);
	  GB_FRIEND_BINARY_OPERATOR_DECLAR(string, +, const string&, const char);
	  GB_FRIEND_BINARY_OPERATOR_DECLAR(string, +, const string&, const int);
	  GB_FRIEND_BINARY_OPERATOR_DECLAR(string, +, const string&, const unsigned int);
	  GB_FRIEND_BINARY_OPERATOR_DECLAR(string, +, const string&, const float);

	  //non-const rvalue ref version
	  GB_FRIEND_BINARY_OPERATOR_DECLAR(string, +, string&&, const char*);
	  GB_FRIEND_BINARY_OPERATOR_DECLAR(string, +, string&&, const char);
	  GB_FRIEND_BINARY_OPERATOR_DECLAR(string, +, string&&, const int);
	  GB_FRIEND_BINARY_OPERATOR_DECLAR(string, +, string&&, const unsigned int);
	  GB_FRIEND_BINARY_OPERATOR_DECLAR(string, +, string&&, const float);
	  
	  void operator+=(const char* szStr);
	  void operator+=(const char val);
	  void operator+=(const int val);
	  void operator+=(const unsigned int val);
	  void operator+=(const float val);
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
