/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#include "filesystem.h"

#include <limits.h>
#include <cassert>
#include <cstdio>
#include <cstring>

#ifdef FS_VESTA_OS_UNIX
#include <fts.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#elif defined(FS_VESTA_OS_WINDOWS)
#include <windows.h>
#endif

#include "logger.h"
#include "string.h"

using namespace fs::sun;

#define FS_SUN_FILESYSTEM_MAX_PATH 128

Filesystem::Filesystem() {
  char path[FS_SUN_FILESYSTEM_MAX_PATH] = {'\0'};
#ifdef FS_VESTA_OS_WINDOWS
  if (GetModuleFileName(NULL, path, FS_SUN_FILESYSTEM_MAX_PATH) == 0)
    FS_SUN_ERROR("Failed to get executable path.");
#elif defined(FS_VESTA_OS_UNIX)
  if (::readlink("/proc/self/exe", path, FS_SUN_FILESYSTEM_MAX_PATH) == -1)
    FS_SUN_ERROR("Failed to get executable path.");
#else
  // TODO
#endif
  executable_path_ = path;
  executable_dir_ = string::DirName(executable_path_.c_str());
}

std::vector<std::string> Filesystem::GetFilesInDir(
    const char* dir, const std::unordered_set<std::string>& suffixes,
    const bool recursively) const {
  std::vector<std::string> files;
  assert(dir != nullptr);
  const std::string str_dir(dir);
  const std::size_t suffixes_count = suffixes.size();
#ifdef FS_VESTA_OS_WINDOWS
  HANDLE h_find = INVALID_HANDLE_VALUE;
  WIN32_FIND_DATA ffd;
  std::string path = string::JoinPath(dir, "*");
  h_find = FindFirstFile(path.c_str(), &ffd);
  if (h_find == INVALID_HANDLE_VALUE) return files;
  do {
    const std::string file(ffd.cFileName);
    if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      if (suffixes_count != 0) {
        const auto idx = file.find_last_of('.');
        if (idx != std::string::npos) {
          const std::string suf{file.substr(idx)};
          if (suffixes.find(suf) != suffixes.end())
            files.push_back(string::JoinPath(str_dir, file));
        }
      } else
        files.push_back(string::JoinPath(str_dir, file));
    } else {
      if (recursively && file != "." && file != "..") {
        const std::vector<std::string> subDirFiles =
            GetFilesInDir(str_dir.c_str(), suffixes, recursively);
        files.insert(files.end(), subDirFiles.begin(), subDirFiles.end());
      }
    }
  } while (FindNextFile(h_find, &ffd) != 0);
#elif defined(FS_VESTA_OS_UNIX)
  // char* const dir_cptr[2] = {new char[std::strlen(dir) + 1], nullptr};
  // std::strcpy(dir_cptr[0], dir);
  // FTS* fts_dir = fts_open(dir_cptr, FTS_NOCHDIR, nullptr);
  // delete[] dir_cptr[0];
  // if (fts_dir == nullptr) return files;
  // FTSENT* fts_ent = fts_read(fts_dir);
  // if (fts_ent == nullptr) return files;
  // do {
  //   if (fts_ent->fts_name[0] == '.')
  //     fts_set(fts_dir, fts_ent, FTS_SKIP);
  //   else if (!recursively && fts_ent->fts_info != FTS_F)
  //     fts_set(fts_dir, fts_ent, FTS_SKIP);
  //   else {
  //     if (suffixes_count != 0) {
  //       std::string file{fts_ent->fts_name};
  //       const auto idx = file.find_last_of('.');
  //       if (idx != std::string::npos) {
  //         const std::string suf{file.substr(idx)};
  //         if (suffixes.find(suf) != suffixes.end())
  //           files.push_back(
  //               string::JoinPath(str_dir.c_str(), fts_ent->fts_name));
  //       }
  //     } else
  //       files.push_back(string::JoinPath(str_dir.c_str(),
  //       fts_ent->fts_name));
  //   }
  //   fts_ent = fts_read(fts_dir);
  // } while (fts_ent != nullptr);
  // fts_close(fts_dir);
  // TODO
  FS_SUN_UNUSED(recursively, suffixes_count);
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
#ifdef FS_VESTA_OS_UNIX
  if (::getcwd(wd, FS_SUN_FILESYSTEM_MAX_PATH) != nullptr) return wd;
#else
#warning TODO
#endif
  return "";
}

std::string Filesystem::GetAbsolutePath(const char* path) const {
  char abs_path[FS_SUN_FILESYSTEM_MAX_PATH]{};
#ifdef FS_SUN_OS_LINUX
  if (::realpath(path, abs_path) != nullptr) return abs_path;
#else
#warning TODO
#endif
  return "";
}
