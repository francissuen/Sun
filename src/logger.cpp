#include "logger.h"
#include <cassert>
//SingletonDefine(gbLog)

using gb::utils::logger;

logger::logger():
    _log_color_code(GB_LOGGER_DEFAULT_LOG_COLOR_CODE),
    _error_color_code(GB_LOGGER_DEFAULT_ERROR_COLOR_CODE),
    _warning_color_code(GB_LOGGER_DEFAULT_WARNING_COLOR_CODE),
    _progress_color_code{
    GB_LOGGER_DEFAULT_PROGRESS_COLOR_CODE,
	GB_LOGGER_DEFAULT_PROGRESS_BAR_COLOR_CODE},
    _log_default_streambuf(std::cout.rdbuf()),
    _error_default_streambuf(std::cerr.rdbuf()),
    _progress_bar_char(GB_LOGGER_DEFAULT_PROGRESS_BAR_CHAR)
    _progress_bar_width(GB_LOGGER_DEF)
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

void logger::progress(const float value, const char* title)
{
    assert(value >= 0.0f && value <= 1.0f);
    
    const std::string& c0 = GB_LOGGER_COLOR_BEGIN + _progress_color_code[0] + GB_LOGGER_COLOR_END;
    const std::string& c1 = GB_LOGGER_COLOR_BEGIN +  _progress_color_code[1] + GB_LOGGER_COLOR_END;

    std::cout << c0 << title << '[' << c1;
    const std::uint8_t width = value * width;
    for(int i = 0; i < width; i++)
	std::cout << _progress_bar_char;
    const std::uint8_t width_left = _progress_bar_width - width;
    for(int i = 0; i < width_left; i++)
	std::cout << ' ';

    std::cout << c0 << ']' << int(value * 100) << '%' << GB_LOGGER_COLOR_BACKTONORMAL << '\r';

    std::cout.flush();
}

void logger::set_progress_bar_char(const char barChar)
{
    _progress_bar_char = barChar;
}
void logger::set_progress_bar_width(const std::uint8_t width)
{
    _progress_bar_width = width;
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
