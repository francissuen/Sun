/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_FILESYSTEM_H
#define FS_SUN_FILESYSTEM_H

#include <filesystem>
#include <string>
#include <unordered_set>
#include <vector>

#include "config.h"
#include "os.h"
#include "singleton.h"

FS_SUN_NS_BEGIN

class FS_SUN_API Filesystem {
  FS_SUN_SINGLETON(Filesystem)
 private:
  Filesystem();

 public:
  /**
   * \brief Get all the files in \b dir with suffix contained in \b suffixes
   * recursively if \b recursively is true.
   *
   * \note If \b suffixes is empty, then all the files in \b dir will be
   * returned.
   */
  std::vector<std::string> GetFilesInDir(
      const char* dir, const std::unordered_set<std::string>& extensions,
      const bool recursively = false) const;

  template <typename TFunc>
  void IterateDir(const char* dir, const TFunc& func,
                  const bool recursively = false) const {
    const auto& iterating = [&func](const auto& dir_itr) {
      for (const auto& d_e : dir_itr) {
        func(d_e);
      }
    };
    recursively ? iterating(std::filesystem::recursive_directory_iterator(dir))
                : iterating(std::filesystem::directory_iterator(dir));
  }
  const std::string& GetExecutablePath() const;
  const std::string& GetExecutableDir() const;
  std::string GetWorkingDir() const;
  std::string GetAbsolutePath(const char* path) const;

 private:
  std::string executable_path_;
  std::string executable_dir_;
};

FS_SUN_NS_END

#endif  // FS_SUN_FILESYSTEM_H
