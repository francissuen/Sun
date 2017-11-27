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

#define _GB_UTILS_STRING_OPERATOR_PLUS_DECLARE(type)			\
	  /*+=*/							\
	      void operator+=(type);					\
	      /*const version*/						\
	      GB_FRIEND_BINARY_OPERATOR_DECLARE(string, +, const string&, type); \
	      /*non-const rvalue ref version*/				\
	      GB_FRIEND_BINARY_OPERATOR_DECLARE(string, +, string&&, type); 
	  
	  _GB_UTILS_STRING_OPERATOR_PLUS_DECLARE(const char*);
	  _GB_UTILS_STRING_OPERATOR_PLUS_DECLARE(const char);
	  _GB_UTILS_STRING_OPERATOR_PLUS_DECLARE(const int);
	  _GB_UTILS_STRING_OPERATOR_PLUS_DECLARE(const unsigned int);
	  _GB_UTILS_STRING_OPERATOR_PLUS_DECLARE(const float);
	  _GB_UTILS_STRING_OPERATOR_PLUS_DECLARE(const long);
	  _GB_UTILS_STRING_OPERATOR_PLUS_DECLARE(const unsigned long);
	  
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
