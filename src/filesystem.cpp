/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#include "filesystem.h"
#include "logger.h"
#include "string.h"
#include "debug.h"
#ifdef __GNUC__
#include <unistd.h>
#elif _MSC_VER
#include <windows.h>
#endif

using namespace fs::sun;

#define FS_SUN_FILESYSTEM_MAX_PATH 128

Filesystem::Filesystem():
#ifdef _MSC_VER
    dir_separator_('\\')
#else
    dir_separator_('/')
#endif
{
    char path[FS_SUN_FILESYSTEM_MAX_PATH] = {'\0'};
#ifdef _MSC_VER
    GetModuleFileName(NULL, path, FS_SUN_FILESYSTEM_MAX_PATH);
#elif __GNUC__
    FS_SUN_ASSERT(::readlink("/proc/self/exe", path, FS_SUN_FILESYSTEM_MAX_PATH) != -1);
#endif
    working_dir_ = path;
    const auto lastSepIdx = working_dir_.find_last_of(dir_separator_);
    if( lastSepIdx != std::string::npos)
    {
        working_dir_ = working_dir_.substr(0, lastSepIdx);
    }
    else
        FS_SUN_ASSERT(false);
}

std::vector<std::string> Filesystem::GetFilesInDir(
    const char* dir,
    const std::unordered_set<std::string> & suffixes,
    const bool recursively)const
{
    FS_SUN_ASSERT(dir != nullptr);
    std::vector<std::string> files;
#ifdef _MSC_VER
    HANDLE hFind  = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;
    const std::string strDir(dir);
    std::string path = strDir + dir_separator_ + '*';
    hFind = FindFirstFile(path.c_str(), &ffd);
    if(hFind == INVALID_HANDLE_VALUE)
        return files;
    const std::size_t suffixesCount = suffixes.size();
    do
    {
        const std::string file(ffd.cFileName);
        if(! (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            if(suffixesCount != 0)
            {
                const auto idx = file.find_last_of('.');
                if(idx != std::string::npos)
                {
                    const std::string suf(file.substr(idx));
                    if(suffixes.find(suf) != suffixes.end())
                        files.push_back(strDir + dir_separator_ + file);
                }
            }
            else
                files.push_back(strDir + dir_separator_ + file);
        }
        else
        {
            if(recursively && file != "." && file != "..")
            {
                const std::vector<std::string> subDirFiles = GetFilesInDir(strDir.c_str(),
                                                                           suffixes,
                                                                           recursively);
                files.insert(files.end(), subDirFiles.begin(), subDirFiles.end());
            }
        }
    }
    while(FindNextFile(hFind, &ffd) != 0);
#endif

    return files;
}

std::string Filesystem::GetAbsolutePath(const char* szPath)const
{
    FS_SUN_ASSERT(szPath != nullptr);
#ifdef _MSC_VER
    if(szPath[1] == ':')
#elif __GNUC__
        if(szPath[0] == '/')
#endif
            return std::string(szPath);
        else//relative path
            return working_dir_ + dir_separator_ + szPath;
}
