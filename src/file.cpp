#include "file.h"
#include "string.h"
using gb::utils::file;
using gb::utils::string;

file::file(const char* filePath):
    _file(filePath, std::ios_base::in | std::ios_base::out | std::ios_base::binary)
{
    if(!_file.is_open())
	throw string("file open failed@") + filePath;

    _file.seekg(0, std::ios_base::end);

    _size = _file.tellg();
}

file::~file()
{
    _file.close();
}
void file::read(char* const buffer, const size_t bufferSize)
{
    assert(buffer != nullptr);
    _file.read(buffer, bufferSize);
}

void file::write(const char* data, const size_t size)
{
    assert(data != nullptr);
    _file.write(data, size);
}
