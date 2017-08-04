#include "gbLog.h"
#include <stdlib.h>
//SingletonDefine(gbLog)
#ifdef WIN32
HANDLE gbLog::hConsole = ::GetStdHandle(STD_OUTPUT_HANDLE);
#endif
 void gbLog::Log(const char* str)
 {
	 if (str == nullptr)
		 str = "Log str error";
 	gbLocalTime(buffer);
	#ifdef WIN32
	::SetConsoleTextAttribute(hConsole, gb_LOG_LOG_COLOR);
	#endif
 	std::cout << buffer << std::endl << "LOG:" << str << std::endl;

	//	std::cou
 }

// void gbLog::Log(const wchar_t * wstr)
// {
// 	gbLocalTime(buffer);
// 	::SetConsoleTextAttribute(hConsole, gb_LOG_LOG_COLOR);
// 	std::wcout << buffer << std::endl << "LOG:" << wstr << std::endl;
// }

 void gbLog::Error(const char * str)
 {
	 if (str == nullptr)
		 str = "Error str error";
 	gbLocalTime(buffer);
	::SetConsoleTextAttribute(hConsole, gb_LOG_ERROR_COLOR);
 	std::cerr << buffer << std::endl << "ERROR:" << str << std::endl;
 }
void gbLog::Warning(const char* msg)
{
	gbLocalTime(buffer);
	::SetConsoleTextAttribute(hConsole, gb_LOG_WARNING_COLOR);
	std::cout << buffer << std::endl << "WARNING:" << msg << std::endl;
}

#ifdef gbLUAAPI
void gbLog::gb_LC_Reg(lua_State* L)
{
	gb_LC_Func_Def;
	gb_LC_Func_push("Log", &gbLog::gb_LC_EF_Log);
	gb_LC_Func_push("Error", &gbLog::gb_LC_EF_Error);
	gb_LC_Func_push("Warning", &gbLog::gb_LC_EF_Warning);

	gbLuaCPP_PrvCns<gbLog>::Register(L, "gbLog", funcs);

	gb_LC_Singleton_Instance_Reg(gbLog);
}
#endif
