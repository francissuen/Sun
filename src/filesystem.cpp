/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#include "filesystem.h"

using namespace fs::Sun;

#define _FS_SUN_FILESYSTEM_MAX_PATH 128

filesystem::filesystem():
#ifdef _MSC_VER
    _directorySeparator('\\')
#else
    _directorySeparator('/')
#endif
{
    char path[_FS_SUN_FILESYSTEM_MAX_PATH] = {'\0'};
#ifdef _MSC_VER
    GetModuleFileName(NULL, path, _FS_SUN_FILESYSTEM_MAX_PATH);
#elif __GNUC__
    FS_SUN_ASSERT(::readlink("/proc/self/exe", path, _FS_SUN_FILESYSTEM_MAX_PATH) != -1);
#endif
    _workingDir = path;
    _workingDir = _workingDir.substr_at_lhs_of_last(_directorySeparator, true);
}

std::vector<string> filesystem::get_files_in_dir(const char* dir,
                                                 const std::unordered_set<string> & suffixes,
                                                 const bool recursively)const
{
    FS_SUN_ASSERT(dir != nullptr);
    std::vector<string> files;
#ifdef _WIN32
    HANDLE hFind  = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;
    const string strDir(dir);
    string path = strDir + _directorySeparator + '*';
    hFind = FindFirstFile(path, &ffd);
    if(hFind == INVALID_HANDLE_VALUE)
    {
	logger::Instance().warning((string)"filesystem::get_files_in_dir, hFind == INVALID_HANDLE_VALUE @dir: " + strDir);
	return files;
    }
    const std::size_t suffixesCount = suffixes.size();
    do
    {
        string file(ffd.cFileName);
        if(! (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            if(suffixesCount != 0)
            {
                string suf(file.substr_at_rhs_of_last('.'));
                if(suffixes.find(suf) != suffixes.end())
                    files.push_back(strDir + _directorySeparator + file);
                else
                    continue;
            }
            else
                files.push_back(strDir + _directorySeparator + file);
        }
        else
        {
            if(recursively && file != "." && file != "..")
            {
                const std::vector<string> subDirFiles = get_files_in_dir(strDir,
                                                                         suffixes,
                                                                         recursively);
                files.insert(files.end(), subDirFiles.begin(), subDirFiles.end());
            }
            else
                continue;
        }
    }
    while(FindNextFile(hFind, &ffd) != 0);
#endif

    return files;
}

string filesystem::get_absolute_path(const char* szPath)const
{
    assert(szPath != nullptr);
#ifdef _MSC_VER
    if(szPath[1] == ':')
#elif __GNUC__
        if(szPath[0] == '/')
#endif
            return string(szPath);
        else//relative path
            return _workingDir + _directorySeparator + szPath;
}
