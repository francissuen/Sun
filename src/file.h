/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include "string.h"
#include <fstream>

FS_SUN_NS_BEGIN

	FS_SUN_CLASS file
	{
	public:
	    file(const char* filePath, bool bRead = true);
	    ~file();
	    void read(void* const buffer, const size_t bufferSize);
	    void write(const void* data, const size_t size);
	    void close();
	    
	    bool eof()const { return _file.eof(); }
	    std::uint32_t size() const { return _size; }
	private:
	    std::fstream _file;
	    std::uint32_t _size;
	    string _filePath;
	};

FS_SUN_NS_END
