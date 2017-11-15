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

//#include "../RenderCore/LuaCPP/gbLuaCPP.h"

#define GB_FILESYSTEM_MAX_PATH 128

namespace gb
{
    namespace utils
    {
	gbUtilsEXPORT_CLASS filesystem
	{
	    SingletonDeclare(filesystem)
		public:
		///if suffix is nullptr then all files here will be output 
		void get_files_here(const char* path, std::vector<std::string>& files, const std::vector<const char*>* suffix = nullptr);
	    void get_work_path(gb::utils::string& buffer);

	    // gb_LC_EXPORT_WRAP_FUNC(GetWorkPath, 
	    // {
	    // 	gbString val;
	    // GetWorkPath(val);
	    // lua_pushstring(L, (const char*)val);
	    // return 1;
	    // });

	    // gb_LC_Register_PrvCns(filesystem);
	};
	
    };
};
