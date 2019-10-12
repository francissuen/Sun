/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include "config.h"
#include "singleton.h"
#include <vector>
#include <unordered_set>
#include <string>
FS_SUN_NS_BEGIN

FS_SUN_CLASS Filesystem: public Singleton<Filesystem>
{
private:
    Filesystem();
public:

    /**
     * \brief Get all the files in \b dir with suffix contained in \b suffixes
     * recursively if \b recursively is true.
     * 
     * \note If \b suffixes is empty, then all the files in \b dir will be returned.
     */
    std::vector<std::string> GetFilesInDir(const char* dir,
                                           const std::unordered_set<std::string> & suffixes,
                                           const bool recursively = false)const;
    
    inline const std::string& GetWorkingDir()const{ return working_dir_; }

    /**
     * \param szPath If szPath[0] == '/'(linux) or szPath[1] == ':'(win), then it's an absolute path, 
     * else it's a relative path.
     */
    std::string GetAbsolutePath(const char* szPath)const;
private:
    std::string working_dir_;
    const char dir_separator_;
};
	
FS_SUN_NS_END
