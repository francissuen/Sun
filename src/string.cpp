/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#include <cassert>
#include <cstring>

#include "logger.h"
#include "string.h"

FS_SUN_NS_BEGIN
namespace string {

static const char path_separator{
#ifdef FS_SUN_WINDOWS
    '\\'
#else
    '/'
#endif
};

std::vector<std::string> ExtractBlocks(
    const std::string& src,
    const std::vector<std::pair<std::string, std::string>>&
        bounding_delimiters) {
  std::vector<std::string> ret;

  for (const auto& bd : bounding_delimiters) {
    const std::string& first = bd.first;
    const std::size_t begin_pos = src.find(first, 0);
    const std::size_t delrs_len = first.size();

    if (begin_pos != std::string::npos) {
      const std::size_t block_begin_pos = begin_pos + delrs_len;

      const std::size_t end_pos = src.find(bd.second, block_begin_pos);
      if (end_pos != std::string::npos) {
        ret.push_back(src.substr(block_begin_pos, end_pos - block_begin_pos));
      }
    }
  }

  return ret;
}

std::vector<std::string> Split(const std::string& src, const char* delimeter) {
  assert(delimeter != nullptr);
  std::vector<std::string> ret;
  std::size_t cur_pos = 0;
  const std::size_t delrs_len = std::strlen(delimeter);
  while (true) {
    const std::size_t pos = src.find(delimeter, cur_pos);
    if (pos != std::string::npos) {
      ret.push_back(src.substr(cur_pos, pos));
      cur_pos += (pos + delrs_len);
    } else {
      ret.push_back(src.substr(cur_pos));
      break;
    }
  }
  return ret;
}

std::string DirName(const char* path) {
  const std::string str_path{path};
  const std::size_t pos = str_path.find_last_of(path_separator);
  return str_path.substr(0, pos);
}

std::string FileName(const char* path) {
  const std::string str_path{path};
  const std::size_t pos = str_path.find_last_of(path_separator);
  return str_path.substr(pos);
}

std::string JoinPath(const char* dir, const char* path) {
  return std::string{dir} + path_separator + path;
}

std::string FileExtension(const char* path) {
  std::string str_path{path};
  const std::size_t pos = str_path.find_last_of('.');
  if (pos != std::string::npos)
    return str_path.substr(pos + 1);
  else
    return "";
}

std::array<std::string, 2> SplitIntoDirNameAndFileName(const char* path) {
  const std::string str_path{path};
  const std::size_t pos = str_path.find_last_of(path_separator);
  return {str_path.substr(0, pos), str_path.substr(pos)};
}

bool StartsWith(const char* string_0, const char* string_1) {
  const std::size_t len_string_1 = std::strlen(string_1);
  if (std::strlen(string_0) >= len_string_1) {
    for (std::size_t i = 0; i < len_string_1; i++) {
      if (string_0[i] != string_1[i]) return false;
    }
  } else
    return false;
  return true;
}

}  // namespace string

FS_SUN_NS_END

