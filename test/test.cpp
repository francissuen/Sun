#include <iostream>
#include "../src/common.h"
#include "../src/string.h"
#include "../src/logger.h"
//#include "string_test.cpp"
//#include "args_test.cpp"
//#include "logger_test.cpp"
//#include "filesystem_test.cpp"
//#include "concurrency_test.cpp"
//#include "file_test.cpp"
// #include "luatable_test.cpp"

#define test(test_func, ...)					\
    if(test_func(__VA_ARGS__) == 0)				\
	std::cout << #test_func << " succeeded" << std::endl;	\
    else							\
	std::cout << #test_func << " failed" << std::endl; 

int main(int argc, char** argv)
{
    //_FS_SUN_FUNC_LOG_("main", "msg");
    //_FS_SUN_FUNC_LOG_(__FUNCTION__ FS_SUN_COMMA__VA_ARGS__("ads"))
    FS_SUN_MACRO_CALL(_FS_SUN_FUNC_LOG_, (__FUNCTION__ FS_SUN_COMMA__VA_ARGS__("ads")))

    //{ fs::Sun::string msg("@function: "); msg += __FUNCTION__; if (_FS_SUN_MACRO_CALL_(_FS_SUN_GET32TH_ARGS_, (_FS_SUN_MACRO_CALL_(_FS_SUN_EXCLUDE_FIRST_ARG_, (, "aa", 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)))) > 0) { msg += " @param"; msg = msg.concat_with_delimiter(" : ", "aa"); } fs::Sun::logger::Instance().log(msg); };

 //   try
 //   {
	//test(args_test, argc, argv);
	//test(string_test);
	//test(logger_test);
	//test(filesystem_test);
	//test(concurrency_test);
	//test(file_test);
	//// test(luatable_test);
 //   }
 //   catch(string& err)
 //   {
	//logger::Instance().error(err);
 //   }
 //   catch(...)
 //   {
	//logger::Instance().error("unknown error");
 //   }

	//::getchar();
    return 0;
}
