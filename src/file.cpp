/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#include <cassert>

#include "file.h"
#include "logger.h"

using namespace fs::sun;

File::File(const char* file_path, const char* open_mode)
    : path_{file_path}, file_{std::fopen(file_path, open_mode)} {
  if (file_ != nullptr) {
    if (std::fseek(file_, 0, SEEK_END) == 0) {
      const long size = std::ftell(file_);
      if (size != -1l) {
        if (std::fseek(file_, 0, SEEK_SET) == 0) size_ = size;
      }
    }
  }
}

File::~File() {
  if (file_ != nullptr) {
    if (std::fclose(file_) != 0)
      cout("Failed to close file: " + path_, Logger::S_ERROR);
  }
}

std::string File::GetPath() const { return path_; }

std::size_t File::GetSize() const { return size_; }

bool File::IsGood() const { return size_ != 0u; }

std::vector<char> File::Read() {
  std::vector<char> ret;
  assert(file_ != nullptr && size_ != 0u);
  ret.resize(size_);
  if (std::fread(ret.data(), sizeof(char), size_, file_) != size_) ret.clear();
  return ret;
}

bool File::Read(char* buffer, const std::size_t size) {
  assert(file_ != nullptr && size_ != 0u && buffer != nullptr);
  if (std::fread(buffer, sizeof(char), size, file_) == size)
    return true;
  else
    return false;
}
