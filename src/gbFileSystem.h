#pragma once
#include "gbCommon.h"
#include <vector>
#include <string>
#include "gbLog.h"
#include "gbString.h"
#ifdef __GNUC__
#include <unistd.h>
#elif _MSC_VER
#include <windows.h>
#endif

//#include "../RenderCore/LuaCPP/gbLuaCPP.h"

#define gb_FILESYSTEM_MAX_PATH 128

gbUtilsEXPORT_CLASS gbFileSystem
{
  SingletonDeclare(gbFileSystem)
  public:
  ///if suffix is nullptr then all files here will be output 
  void GetFilesHere(const char* path, std::vector<std::string>& files, const std::vector<const char*>* suffix = nullptr);
  void GetWorkPath(gbString& buffer);

  // gb_LC_EXPORT_WRAP_FUNC(GetWorkPath, 
  // {
  // 	gbString val;
  // GetWorkPath(val);
  // lua_pushstring(L, (const char*)val);
  // return 1;
  // });

  // gb_LC_Register_PrvCns(gbFileSystem);
};
