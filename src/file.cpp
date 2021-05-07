/* Copyright (C) 2021 Francis Sun, all rights reserved. */

#include <cassert>

#include "file.h"
#include "logger.h"

using namespace fs::sun;

File::File(const char* file_path) : path_{file_path}, file_{nullptr} {}

File::File(File&& other)
    : path_{std::move(other.path_)}, file_{other.file_}, size_{other.size_} {
  other.file_ = nullptr;
  other.size_ = 0u;
}

File& File::operator=(File&& other) {
  path_ = std::move(other.path_);
  file_ = other.file_;
  size_ = other.size_;
  other.file_ = nullptr;
  other.size_ = 0u;
  return *this;
}

File::~File() { Close(); }

bool File::Open(const char* open_mode) {
  if (file_ == nullptr) {
    file_ = std::fopen(path_.c_str(), open_mode);
    if (file_ != nullptr) {
      if (std::fseek(file_, 0, SEEK_END) == 0) {
        const long size = std::ftell(file_);
        if (size != -1l) {
          if (std::fseek(file_, 0, SEEK_SET) == 0) {
            size_ = size;
            return true;
          }
        }
      }
    }
  }
  return false;
}

bool File::Close() {
  if (file_ != nullptr) {
    if (std::fclose(file_) == 0) {
      file_ = nullptr;
      return true;
    }
  }
  return false;
}

std::string File::GetPath() const { return path_; }

std::size_t File::GetSize() const { return size_; }

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
