/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#include "file.h"
#include "string.h"
using namespace fs::Sun;

file::file(const char* filePath, bool bRead):
    _size(0),
    _filePath(filePath)
{
    std::ios_base::openmode openmode = std::ios_base::binary;
    if(bRead)
	openmode = openmode | std::ios_base::in;
    else
	openmode = openmode | std::ios_base::out;

    _file = std::fstream(filePath, openmode);

    if(bRead)
    {
	_file.seekg(0, std::ios_base::end);

	_size = (std::uint32_t)_file.tellg();

	_file.seekg(0, std::ios_base::beg);
    }
}

file::~file()
{
    _file.close();
}

void file::close()
{
    _file.close();
}
bool file::read(void* const buffer, const size_t bufferSize)
{
    assert(buffer != nullptr);
    _file.read((char* const)buffer, bufferSize);
    return ! _file.fail();
}

bool file::write(const void* data, const size_t size)
{
    assert(data != nullptr);
    _file.write((const char*)data, size);
    return ! _file.fail();
}
