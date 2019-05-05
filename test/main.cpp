#include "../src/logger.h"

using namespace fs::Sun;

int main(int argc, char ** argv)
{
    cout("hello world!", logger::S_VERBOSE);
    cout("hello world!", logger::S_DEBUG);
    cout("hello world!", logger::S_INFO);
    cout("hello world!", logger::S_WARNING);
    cout("hello world!", logger::S_ERROR);
    cout("hello world!", logger::S_FATAL);
    cout.flush();
    ::getchar();
    return 0;
}
