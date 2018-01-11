#include <iostream>
#include <cassert>
#include "../src/string.h"

using namespace gb::utils;

void print_str(const string str)
{
    std::cout <<(const char*) str << std::endl;
}


int string_test()
{
 
    print_str(string("hello") + "world");
    print_str(string("hello") + 1);
    print_str(string("hello") + -1);
    print_str(string("hello") + 0.1f);

    assert((string("hello") + "world") == "helloworld");
    assert((string("hello") + "world" + 'x') != "helloworld");

    print_str("hello" + string("world"));

	string str = "world";
	print_str(string("hello") + str);

    return 0;
}
