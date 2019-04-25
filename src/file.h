/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include "string.h"
#include <fstream>

FS_SUN_NS_BEGIN

FS_SUN_CLASS ifile: public std::ifstream
{
public:
    std::size_t size() const;
};

FS_SUN_CLASS ofile: public std::ofstream
{
    
};

FS_SUN_CLASS file
{
public:
file(const char* filePath, bool bRead = true);
~file();
bool read(void* const buffer, const size_t bufferSize);
bool write(const void* data, const size_t size);
void close();
	    
inline bool eof()const { return _file.eof(); }
inline bool is_open() const { return _file.is_open(); }
std::uint32_t size() const { return _size; }
private:
std::fstream _file;
std::uint32_t _size;
string _filePath;
};

FS_SUN_NS_END
