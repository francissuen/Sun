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
  executable_dir_ = string::DirName(executable_path_.c_str());
}

std::vector<std::string> Filesystem::GetFilesInDir(
    const char* dir, const std::unordered_set<std::string>& suffixes,
    const bool recursively) const {
  std::vector<std::string> files;
  assert(dir != nullptr);
#ifdef FS_SUN_WINDOWS
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

const std::string& Filesystem::GetExecutablePath() const {
  return executable_path_;
}
const std::string& Filesystem::GetExecutableDir() const {
  return executable_dir_;
}
std::string Filesystem::GetWorkingDir() const {
  char wd[FS_SUN_FILESYSTEM_MAX_PATH]{};
#ifdef FS_SUN_LINUX
  if (::getcwd(wd, FS_SUN_FILESYSTEM_MAX_PATH) != nullptr) return wd;
#else
#error TODO
#endif
  return "";
}

std::string Filesystem::GetAbsolutePath(const char* path) const {
  char abs_path[FS_SUN_FILESYSTEM_MAX_PATH]{};
#ifdef FS_SUN_LINUX
  if (::realpath(path, abs_path) != nullptr) return abs_path;
#else
#error TODO
#endif
  return "";
}
