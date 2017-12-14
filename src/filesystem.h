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

GB_UTILS_NS_BEGIN

	GB_UTILS_CLASS filesystem
	{
	    GB_SINGLETON_EXCLUDECTOR(filesystem);
	    filesystem();
	public:
	    ///if suffix is nullptr then all files here will be output 
	    std::vector<gb::utils::string>  get_files_here(const char* path, const std::vector<const char*>* suffix = nullptr)const;
	    inline const gb::utils::string& get_workingdir()const{ return _workingDir; }

	    /*
	     *@param, if szPath[0] == '/'(linux) or szPath[1] == ':'(win), then it's a absolute path, 
	     *else it's a relative path.
	     */
	    gb::utils::string get_absolute_path(const char* szPath)const;
//	    _LC_EXPORT_WRAP_FUNC(GetWorkPath, 
	    // {
	    // 	gbString val;
	    // GetWorkPath(val);
	    // lua_pushstring(L, (const char*)val);
	    // return 1;
	    // });

	    // gb_LC_Register_PrvCns(filesystem);
//	};
	private:
	    gb::utils::string _workingDir;
	};
	
GB_UTILS_NS_END