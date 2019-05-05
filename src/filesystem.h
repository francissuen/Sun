/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include "config.h"
#include "common.h"
#include <vector>
#include <unordered_set>
#include <string>
FS_SUN_NS_BEGIN

FS_SUN_CLASS filesystem: public singleton<filesystem>
{
private:
    filesystem();
public:

    /**
     * \brief Get all the files in \b dir with suffix contained in \b suffixes
     * recursively if \b recursively is true.
     * 
     * \note If \b suffixes is empty, then all the files in \b dir will be returned.
     */
    std::vector<std::string> get_files_in_dir(const char* dir,
                                              const std::unordered_set<std::string> & suffixes,
                                              const bool recursively = false)const;
    
    inline const std::string& get_workingdir()const{ return _workingDir; }

    /**
     * \param szPath If szPath[0] == '/'(linux) or szPath[1] == ':'(win), then it's a absolute path, 
     * else it's a relative path.
     */
    std::string get_absolute_path(const char* szPath)const;
private:
    std::string _workingDir;
    const char _directorySeparator;
};
	
FS_SUN_NS_END
