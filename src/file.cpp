#include "file.h"
#include "string.h"
using gb::utils::file;
using gb::utils::string;

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
    if(!_file.is_open())
	throw string("file open failed@") + filePath;

    if(bRead)
    {
	_file.seekg(0, std::ios_base::end);

	_size = _file.tellg();

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
void file::read(char* const buffer, const size_t bufferSize)
{
    assert(buffer != nullptr);
    _file.read(buffer, bufferSize);
    if(!_file)
    	throw string("file read error@") + _filePath;
}

void file::write(const char* data, const size_t size)
{
    assert(data != nullptr);
    _file.write(data, size);
    if(!_file)
	throw string("file write error@") + _filePath;
}
