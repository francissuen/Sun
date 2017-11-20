#include "logger.h"
#include <cassert>
//SingletonDefine(gbLog)

using gb::utils::logger;

logger::logger():
    _log_color_code(GB_LOGGER_DEFAULT_LOG_COLOR_CODE),
    _error_color_code(GB_LOGGER_DEFAULT_ERROR_COLOR_CODE),
    _warning_color_code(GB_LOGGER_DEFAULT_WARNING_COLOR_CODE),
    _log_default_streambuf(std::cout.rdbuf()),
    _error_default_streambuf(std::cerr.rdbuf())
{
#ifdef _MSC_VER
    _hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
#endif
}

void logger::set_log_streambuf(std::streambuf* streambuf)
{
    std::cout.rdbuf(streambuf != nullptr ? streambuf : _log_default_streambuf);
}

void logger::set_error_streambuf(std::streambuf* streambuf)
{
    std::cerr.rdbuf(streambuf != nullptr ? streambuf : _error_default_streambuf);
}

#ifdef _MSC_VER

#define _gb_fancy_print(ostream, title, win_color_code)		\
    assert(szMsg != nullptr);					\
    GB_GET_LOCALTIME(timeBuf);					\
    ::SetConsoleTextAttribute(_hConsole, win_color_code);	\
    ostream << timeBuf << std::endl				\
    << title << szMsg << std::endl;

#elif __GNUC__

#define _gb_fancy_print(ostream, title, color_code)			\
    assert(szMsg != nullptr);						\
    GB_GET_LOCALTIME(timeBuf);						\
    ostream << GB_LOGGER_COLOR_BEGIN + color_code + GB_LOGGER_COLOR_END	\
    << timeBuf << std::endl						\
    << title << szMsg << GB_LOGGER_COLOR_BACKTONORMAL << std::endl;	\

#endif

void logger::log(const char* szMsg)const
{
#ifdef _MSC_VER
    _gb_fancy_print(std::cout, "LOG: ", GB_LOGGER_LOG_MS_COLOR);
#elif __GNUC__
    _gb_fancy_print(std::cout, "LOG: ", _log_color_code);
#endif
}

void logger::set_log_color_code(const char* szCode)
{
    assert(szCode != nullptr);
    _log_color_code = szCode;
}

void logger::error(const char * szMsg)const
{
#ifdef _MSC_VER
    _gb_fancy_print(std::cerr, "ERROR: ", GB_LOGGER_ERROR_MS_COLOR);
#elif __GNUC__
    _gb_fancy_print(std::cerr, "ERROR: ", _error_color_code);
#endif
}
void logger::set_error_color_code(const char* szCode)
{
    assert(szCode != nullptr);
    _error_color_code = szCode;
}
void logger::warning(const char* szMsg)const
{
#ifdef _MSC_VER
    _gb_fancy_print(std::cout, "WARNING: ", GB_LOGGER_WARNING_MS_COLOR);
#elif __GNUC__
    _gb_fancy_print(std::cout, "WARNING: ", _warning_color_code);
#endif
}

void logger::set_warning_color_code(const char* szCode)
{
    assert(szCode != nullptr);
    _warning_color_code = szCode;
}

#ifdef gbLUAAPI
void logger::gb_LC_Reg(lua_State* L)
{
    gb_LC_Func_Def;
    gb_LC_Func_push("log", &log::gb_LC_EF_log);
    gb_LC_Func_push("error", &log::gb_LC_EF_error);
    gb_LC_Func_push("warning", &log::gb_LC_EF_warning);

    gbLuaCPP_PrvCns<logger>::Register(L, "logger", funcs);

    gb_LC_Singleton_Instance_Reg(log);
}
#endif
