#pragma once
#include "common.h"
#include <vector>
#include "logger.h"
#include "string.h"
#ifdef __GNUC__
#include <unistd.h>
#elif _MSC_VER
#include <windows.h>
#endif

//#include "../RenderCore/LuaCPP/fsLuaCPP.h"

FS_COMMON_NS_BEGIN

FS_COMMON_CLASS filesystem
{
    FS_SINGLETON_NO_CTORDEF(filesystem);
public:
    ///if suffix is nullptr then all files here will be output 
    std::vector<fs::common::string>  get_files_here(const char* path, const std::vector<const char*>* suffix = nullptr)const;
    inline const fs::common::string& get_workingdir()const{ return _workingDir; }

    /*
     *@param, if szPath[0] == '/'(linux) or szPath[1] == ':'(win), then it's a absolute path, 
     *else it's a relative path.
     */
    fs::common::string get_absolute_path(const char* szPath)const;
//	    _LC_EXPORT_WRAP_FUNC(GetWorkPath, 
    // {
    // 	fsString val;
    // GetWorkPath(val);
    // lua_pushstring(L, (const char*)val);
    // return 1;
    // });

    // fs_LC_Register_PrvCns(filesystem);
//	};
private:
    fs::common::string _workingDir;
};
	
FS_COMMON_NS_END
