#include <iostream>

#include "string_test.cpp"
#include "args_test.cpp"

int main(int argc, char** argv)
{
    if(args_test(argc, argv) != 0)
	std::cout << "args test failed" << std::endl;
    if(string_test() != 0)
	std::cout << "string test failed" << std::endl;
    return 0;
}
