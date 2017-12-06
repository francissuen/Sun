#pragma once

#include <fstream>

namespace gb
{
    namespace utils
    {
	class file
	{
	public:
	    file(const char* filePath);
	    ~file();
	    void read(char* const buffer, const size_t bufferSize);
	    void write(const char* data, const size_t size);
	    std::uint32_t size() const { return _size; }
	private:
	    std::fstream _file;
	    std::uint32_t _size;
	};
    };
};
