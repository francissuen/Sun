#include "../src/logger.h"

using gb::utils::logger;
int logger_test()
{

    logger::Instance().log("hello world");
#ifdef _MSC_VER
	logger::Instance().set_log_color_code(9);//blue
#elif __GNUC__
	logger::Instance().set_log_color_code("34;40");//blue
#endif
    
    logger::Instance().log("hello world");

    logger::Instance().error("hello hell");
#ifdef _MSC_VER
    logger::Instance().set_error_color_code(13);//magenta
#elif __GNUC__
	logger::Instance().set_error_color_code("35;40");//magenta
    
#endif
	logger::Instance().error("hello hell");

    logger::Instance().warning("hello");
#ifdef _MSC_VER
	logger::Instance().set_warning_color_code(10);//green
#elif __GNUC__
	logger::Instance().set_warning_color_code("32;40");//green
#endif
    logger::Instance().warning("hello");

#ifdef _MSC_VER
	logger::Instance().set_log_color_code(GB_LOGGER_DEFAULT_LOG_MS_COLOR_CODE);
	logger::Instance().set_error_color_code(GB_LOGGER_DEFAULT_ERROR_MS_COLOR_CODE);
	logger::Instance().set_warning_color_code(GB_LOGGER_DEFAULT_WARNING_MS_COLOR_CODE);
#elif __GNUC__
	logger::Instance().set_log_color_code(GB_LOGGER_DEFAULT_LOG_COLOR_CODE);
	logger::Instance().set_error_color_code(GB_LOGGER_DEFAULT_ERROR_COLOR_CODE);
	logger::Instance().set_warning_color_code(GB_LOGGER_DEFAULT_WARNING_COLOR_CODE);
#endif
    return 0;
}
