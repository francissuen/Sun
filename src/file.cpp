/* Copyright (C) 2021 Francis Sun, all rights reserved. */

#include <cassert>

#include "file.h"
#include "logger.h"

using namespace fs::sun;

File::File(const char* file_path) : path_{file_path}, file_{nullptr} {
  if (path_ != "") {
    if (path_[path_.size() - 1] == '/') {
      FS_SUN_ERROR(std::string("file_path can't be a folder: ") + file_path);
      path_ = "";
    }
  }
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
            size_ = static_cast<std::size_t>(size);
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

const std::string& File::GetPath() const { return path_; }

std::size_t File::GetSize() const { return size_; }

std::vector<unsigned char> File::Read() {
  std::vector<unsigned char> ret;
  assert(file_ != nullptr && size_ != 0u);
  ret.resize(size_);
  if (std::fread(ret.data(), sizeof(unsigned char), size_, file_) != size_)
    ret.clear();
  return ret;
}

bool File::Read(unsigned char* buffer, const std::size_t size) {
  assert(file_ != nullptr && size_ != 0u && buffer != nullptr);
  if (std::fread(buffer, sizeof(unsigned char), size, file_) == size)
    return true;
  else
    return false;
}
