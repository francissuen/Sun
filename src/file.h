/* Copyright (C) 2021 Francis Sun, all rights reserved. */

#ifndef FS_SUN_FILE_H
#define FS_SUN_FILE_H

#include <cstdio>
#include <string>
#include <vector>

#include "ns.h"

FS_SUN_NS_BEGIN

class File {
 public:
  File(const char* file_path);

 public:
  ~File();
  File(const File&) = delete;
  File(File&& file);
  File& operator=(const File&) = delete;
  File& operator=(File&&);

 public:
  bool Open(const char* open_mode = "r");
  bool Close();
  std::string GetPath() const;
  std::size_t GetSize() const;
  std::vector<unsigned char> Read();
  bool Read(unsigned char* buffer, const std::size_t size);

 private:
  std::string path_;
  std::FILE* file_;
  std::size_t size_{0u};
};

FS_SUN_NS_END

#endif  // FS_SUN_FILE_H
