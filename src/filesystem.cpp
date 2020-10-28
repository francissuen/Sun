/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#include "filesystem.h"

#include <cassert>
#include <cstdio>

#ifdef FS_SUN_LINUX
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#elif defined(FS_SUN_WINDOWS)
#include <windows.h>
#endif

#include "logger.h"
#include "string.h"

using namespace fs::sun;

#define FS_SUN_FILESYSTEM_MAX_PATH 128

Filesystem::Filesystem() {
  char path[FS_SUN_FILESYSTEM_MAX_PATH] = {'\0'};
#ifdef FS_SUN_WINDOWS
  if (GetModuleFileName(NULL, path, FS_SUN_FILESYSTEM_MAX_PATH) == 0)
    cout("Failed to get executable path.", Logger::S_ERROR);
#elif defined(FS_SUN_LINUX)
  if (::readlink("/proc/self/exe", path, FS_SUN_FILESYSTEM_MAX_PATH) == -1)
    cout("Failed to get executable path.", Logger::S_ERROR);
#else
#error TODO
#endif
  executable_path_ = path;
  const auto lastSepIdx = executable_path_.find_last_of(path_separator_);
  if (lastSepIdx != std::string::npos)
    executable_dir_ = executable_path_.substr(0, lastSepIdx);
}

std::vector<std::string> Filesystem::GetFilesInDir(
    const char* dir, const std::unordered_set<std::string>& suffixes,
    const bool recursively) const {
  std::vector<std::string> files;
  assert(dir != nullptr);
#ifdef _MSC_VER
  HANDLE hFind = INVALID_HANDLE_VALUE;
  WIN32_FIND_DATA ffd;
  const std::string strDir(dir);
  std::string path = strDir + dir_separator_ + '*';
  hFind = FindFirstFile(path.c_str(), &ffd);
  if (hFind == INVALID_HANDLE_VALUE) return files;
  const std::size_t suffixesCount = suffixes.size();
  do {
    const std::string file(ffd.cFileName);
    if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      if (suffixesCount != 0) {
        const auto idx = file.find_last_of('.');
        if (idx != std::string::npos) {
          const std::string suf(file.substr(idx));
          if (suffixes.find(suf) != suffixes.end())
            files.push_back(strDir + dir_separator_ + file);
        }
      } else
        files.push_back(strDir + dir_separator_ + file);
    } else {
      if (recursively && file != "." && file != "..") {
        const std::vector<std::string> subDirFiles =
            GetFilesInDir(strDir.c_str(), suffixes, recursively);
        files.insert(files.end(), subDirFiles.begin(), subDirFiles.end());
      }
    }
  } while (FindNextFile(hFind, &ffd) != 0);
#endif

  return files;
}

std::string Filesystem::GetExecutablePath() const { return executable_path_; }
std::string Filesystem::GetExecutableDir() const { return executable_dir_; }
std::string Filesystem::GetWorkingDir() const {
  std::string ret;
  char wd[FS_SUN_FILESYSTEM_MAX_PATH]{};
#ifdef FS_SUN_LINUX
  if (::getcwd(wd, FS_SUN_FILESYSTEM_MAX_PATH) != nullptr) ret = wd;
#else
#error TODO
#endif
  return ret;
}

char Filesystem::GetPathSeperator() const { return path_separator_; }

std::string Filesystem::GetAbsolutePath(const char* path) const {
  std::string ret;
  char abs_path[FS_SUN_FILESYSTEM_MAX_PATH]{};
#ifdef FS_SUN_LINUX
  if (::realpath(path, abs_path) != nullptr) ret = abs_path;
#else
#error TODO
#endif
  return ret;
}

std::string Filesystem::JoinPath(const char* path_0,
                                 const char* path_1) const {
  return std::string(path_0) + path_separator_ + path_1;
}
