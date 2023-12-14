/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#include "filesystem.h"

#include <limits.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <filesystem>

#ifdef FS_VESTA_OS_WINDOWS
#include <Windows.h>
#elif defined(FS_VESTA_OS_UNIX)
#include <unistd.h>
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
    const char* dir, const std::unordered_set<std::string>& extensions,
    const bool recursively) const {
  std::vector<std::string> files;
  if (dir == nullptr) return files;
  const std::filesystem::path dir_path{dir};
  if (!std::filesystem::is_directory(dir_path)) return files;

  const auto fill_files =
      [&files,
       &extensions](const std::filesystem::directory_entry& de) -> void {
    if (de.is_regular_file()) {
      const std::filesystem::path& de_path = de.path();
      const std::string& de_ext = de_path.extension().string();
      if ((extensions.size() == 0) ||
          (extensions.find(de_ext) != extensions.end()))
        files.push_back(de_path.string());
    }
  };

  IterateDir(dir, fill_files, recursively);

  return files;
}

const std::string& Filesystem::GetExecutablePath() const {
  return executable_path_;
}
const std::string& Filesystem::GetExecutableDir() const {
  return executable_dir_;
}
std::string Filesystem::GetWorkingDir() const {
  return std::filesystem::current_path().string();
}

std::string Filesystem::GetAbsolutePath(const char* path) const {
  return std::filesystem::absolute(path).string();
}
