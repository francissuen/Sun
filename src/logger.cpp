#include "logger.h"
#include <cstdlib>
//SingletonDefine(gbLog)

using gb::utils::logger;

#ifdef WIN32
HANDLE logger::hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
#endif
void logger::log(const char* str)
{
    if (str == nullptr)
	str = "Log str error";
    GB_LocalTime(buffer);
#ifdef WIN32
    ::SetConsoleTextAttribute(hConsole, GB_LOG_LOG_COLOR);
#endif
    std::cout << buffer << std::endl << "LOG:" << str << std::endl;

    //	std::cou
}

// void log::Log(const wchar_t * wstr)
// {
// 	gbLocalTime(buffer);
// 	::SetConsoleTextAttribute(hConsole, gb_LOG_LOG_COLOR);
// 	std::wcout << buffer << std::endl << "LOG:" << wstr << std::endl;
// }

void logger::error(const char * str)
{
    if (str == nullptr)
	str = "Error str error";
    GB_LocalTime(buffer);
#ifdef WIN32
    ::SetConsoleTextAttribute(hConsole, GB_LOG_ERROR_COLOR);
#endif
    std::cerr << buffer << std::endl << "ERROR:" << str << std::endl;
}
void logger::warning(const char* msg)
{
    GB_LocalTime(buffer);
#ifdef WIN32
    ::SetConsoleTextAttribute(hConsole, GB_LOG_WARNING_COLOR);
#endif
    std::cout << buffer << std::endl << "WARNING:" << msg << std::endl;
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
