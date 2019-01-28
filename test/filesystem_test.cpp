#include "../src/filesystem.h"
#include "../src/logger.h"

using namespace fs::Sun;

int filesystem_test()
{
    logger::Instance().log("wd: " + filesystem::Instance().get_workingdir());

    std::vector<string> files = filesystem::Instance().get_files_in_dir(filesystem::Instance().get_workingdir(), {});

    for( auto file : files)
    {
        logger::Instance().log("@file: " + file);
    }
    
    logger::Instance().log("test.sh abs: " + filesystem::Instance().get_absolute_path("test.sh"));
#ifndef _MSC_VER
    logger::Instance().log("/home/test.sh abs: " + filesystem::Instance().get_absolute_path("/home/test.sh"));
#endif
    return 0;
}
