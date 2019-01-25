#include "../src/file.h"
#include "../src/filesystem.h"

using namespace fs::Sun;
#include <fstream>
int file_test()
{
    const string filePath = filesystem::Instance().get_absolute_path("helloworld.txt");
    file wFile(filePath, false);
    if(!wFile.is_open())
        return 1;
    const char* content = "hello world";

    FS_SUN_CALL_V(wFile.write, content, strlen(content))
    FS_SUN_CALL_V(wFile.close)

    file rFile(filesystem::Instance().get_absolute_path(filePath));
    if(!rFile.is_open())
        return 1;
    size_t size = rFile.size() + 100;
    char* buffer = new char[size + 1]{'\0'};
    
    if(!rFile.read(buffer, size))	
    {
	if(rFile.eof())
	    logger::Instance().warning("reached end of file");
        else
            logger::Instance().error("other unknown error");
    }

    logger::Instance().log(buffer);
    return 0;
}
