#include "../src/file.h"
#include "../src/filesystem.h"

using namespace gb::utils;
#include <fstream>
int file_test()
{
    const string filePath = filesystem::Instance().get_absolute_path("helloworld.txt");
    file wFile(filePath, false);
    const char* content = "hello world";
    
    wFile.write(content, strlen(content));
    wFile.close();

    file rFile(filesystem::Instance().get_absolute_path(filePath));
    
    size_t size = rFile.size() + 100;
    char* buffer = new char[size + 1]{'\0'};
    
    try
    {
	rFile.read(buffer, size);	
    }
    catch(string& error)
    {
	if(rFile.eof())
	    logger::Instance().warning("reached end of file");
	else
	    logger::Instance().error(error);
    }

    logger::Instance().log(buffer);
    return 0;
}
