/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include "common.h"
#include <vector>
#include <unordered_set>
#include "logger.h"
#include "string.h"
#ifdef __GNUC__
#include <unistd.h>
#elif _MSC_VER
#include <windows.h>
#endif

//#include "../RenderCore/LuaCPP/fsLuaCPP.h"

FS_SUN_NS_BEGIN

FS_SUN_CLASS filesystem
{
    FS_SUN_SINGLETON_NO_CTORDEF(filesystem);
public:

    /*
     * @desc Get all the files in @param dir with suffix contained in @param suffixes
     * recursively if @param recursively is true.
     * @note If @param suffixes is empty, then all the files in @param dir will be returned.
     */
    std::vector<string> get_files_in_dir(const char* dir,
                                                  const std::unordered_set<string> & suffixes,
                                                  const bool recursively = false)const;
    
    inline const string& get_workingdir()const{ return _workingDir; }

    /*
     *@param if szPath[0] == '/'(linux) or szPath[1] == ':'(win), then it's a absolute path, 
     *else it's a relative path.
     */
    string get_absolute_path(const char* szPath)const;
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
    string _workingDir;
    const char _directorySeparator;
};
	
FS_SUN_NS_END
