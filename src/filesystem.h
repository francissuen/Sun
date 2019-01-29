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

FS_SUN_NS_BEGIN

FS_SUN_CLASS filesystem
{
    FS_SUN_SINGLETON_NO_CTORDEF(filesystem);
public:

    /**
     * \brief Get all the files in \p dir with suffix contained in \p suffixes
     * recursively if \p recursively is true.
     * 
     * \note If \p suffixes is empty, then all the files in \p dir will be returned.
     */
    std::vector<string> get_files_in_dir(const char* dir,
                                                  const std::unordered_set<string> & suffixes,
                                                  const bool recursively = false)const;
    
    inline const string& get_workingdir()const{ return _workingDir; }

    /**
     * \param If szPath[0] == '/'(linux) or szPath[1] == ':'(win), then it's a absolute path, 
     * else it's a relative path.
     */
    string get_absolute_path(const char* szPath)const;
private:
    string _workingDir;
    const char _directorySeparator;
};
	
FS_SUN_NS_END
