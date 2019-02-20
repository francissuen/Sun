#include <iostream>
#include <cassert>
#include "../src/string.h"
#include "../src/logger.h"
using namespace fs::Sun;

int string_test()
{
 
    logger::Instance().log(string("hello: ") + "world");
    logger::Instance().log(string("hello: ") + 1);
    logger::Instance().log(string("hello: ") + -1);
    logger::Instance().log(string("hello: ") + 0.1f);

    assert((string("hello") + "world") == "helloworld");
    assert((string("hello") + "world" + 'x') != "helloworld");

    logger::Instance().log("hello: " + string("world"));

    string str = "world";
    logger::Instance().log(string("hello: ") + str);

    return 0;
}
