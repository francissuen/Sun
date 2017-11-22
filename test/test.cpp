#include <iostream>

#include "string_test.cpp"
#include "args_test.cpp"
#include "logger_test.cpp"
#include "filesystem_test.cpp"
#include "concurrency_test.cpp"

#define test(test_func, ...)					\
    if(test_func(__VA_ARGS__) == 0)				\
	std::cout << #test_func << " succeeded" << std::endl;	\
    else							\
	std::cout << #test_func << " failed" << std::endl; 

int main(int argc, char** argv)
{
    test(args_test, argc, argv);
    test(string_test);
    test(logger_test);
    test(filesystem_test);
    test(concurrency_test);
    return 0;
}
