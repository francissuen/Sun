#include "../src/filesystem.h"
#include "../src/logger.h"

using fs::Sun::logger;
using fs::Sun::filesystem;

int filesystem_test()
{
    logger::Instance().log("wd: " + filesystem::Instance().get_workingdir());

    logger::Instance().log("test.sh abs: " + filesystem::Instance().get_absolute_path("test.sh"));
    logger::Instance().log("/home/test.sh abs: " + filesystem::Instance().get_absolute_path("/home/test.sh"));
    return 0;
}
