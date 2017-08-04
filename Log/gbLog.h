#pragma once
#include <iostream>
#include <string>
#include "../Time/gbTime.h" 
#include "../gbUtilsCommon.h"

#ifdef gbLUAAPI
#include "../LuaCPP/gbLuaCPP.h"
#endif

#include<Windows.h>
// colors are 0=black 1=blue 2=green and so on to 15=white  
// colorattribute = foreground + background * 16
// to get red text on yellow use 4 + 14*16 = 228
// light red on yellow would be 12 + 14*16 = 236
#define gb_LOG_LOG_COLOR 15
#define gb_LOG_ERROR_COLOR 12
#define gb_LOG_WARNING_COLOR 14

#define gb_LOG_MAX_LEN 256

enum gbLogType
  {
    Log = 1,
    Warning, Error
  };

gbUtilsDLL_CLASS  gbLog
{
  SingletonDeclare(gbLog)
public:
    //    void Log(const wchar_t* wstr);
  void Log(const char* msg);
  void Error(const char* msg);
  void Warning(const char* msg);
  
#ifdef gbLUAAPI
  gb_LC_EXPORT_FUNC(Log);
  gb_LC_EXPORT_FUNC(Error);
  gb_LC_EXPORT_FUNC(Warning);
  gb_LC_Register_PrvCns(gbLog);
#endif
 private:
#ifdef WIN32
  static HANDLE hConsole;
#endif
};

