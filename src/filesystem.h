/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_FILESYSTEM_H
#define FS_SUN_FILESYSTEM_H

#include <string>
#include <unordered_set>
#include <vector>

#include "config.h"
#include "platform.h"
#include "singleton.h"
FS_SUN_NS_BEGIN

class FS_SUN_API Filesystem {
  FS_SUN_SINGLETON(Filesystem)

 public:
  /**
   * \brief Get all the files in \b dir with suffix contained in \b suffixes
   * recursively if \b recursively is true.
   *
   * \note If \b suffixes is empty, then all the files in \b dir will be
   * returned.
   */
  std::vector<std::string> GetFilesInDir(
      const char* dir, const std::unordered_set<std::string>& suffixes,
      const bool recursively = false) const;

  const std::string& GetExecutablePath() const;
  const std::string& GetExecutableDir() const;
  std::string GetWorkingDir() const;
  char GetPathSeperator() const;
  std::string GetAbsolutePath(const char* path) const;
  std::string JoinPath(const char* path_0, const char* path_1) const;

 private:
  std::string executable_path_;
  std::string executable_dir_;
  const char path_separator_{
#ifdef FS_SUN_WINDOWS
      '\\'
#else
      '/'
#endif
  };
};

FS_SUN_NS_END

#endif  // FS_SUN_FILESYSTEM_H
