#include <gbUtils/logger.h>

using gb::utils::logger;
int logger_test()
{

    logger::Instance().log("hello world");
    logger::Instance().set_log_color_code("34;40");//blue
    logger::Instance().log("hello world");

    logger::Instance().error("hello hell");
    logger::Instance().set_error_color_code("35;40");//magenta
    logger::Instance().error("hello hell");

    logger::Instance().warning("hello");
    logger::Instance().set_warning_color_code("32;40");//green
    logger::Instance().warning("hello");

    logger::Instance().set_log_color_code(GB_LOGGER_DEFAULT_LOG_COLOR_CODE);
    logger::Instance().set_error_color_code(GB_LOGGER_DEFAULT_ERROR_COLOR_CODE);
    logger::Instance().set_warning_color_code(GB_LOGGER_DEFAULT_WARNING_COLOR_CODE);
    return 0;
}
