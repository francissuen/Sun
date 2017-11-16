#include "../src/string.h"
#include <iostream>
#include <cassert>
using gb::utils::string;
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


    
    return 0;
}
