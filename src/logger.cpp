#include "logger.h"
#include <cassert>
#include <limits>

using gb::utils::logger;
using gb::utils::time;

logger::logger():
    _log_color_code(GB_LOGGER_DEFAULT_LOG_COLOR_CODE),
    _error_color_code(GB_LOGGER_DEFAULT_ERROR_COLOR_CODE),
    _warning_color_code(GB_LOGGER_DEFAULT_WARNING_COLOR_CODE),
    _progress_color_code{
    GB_LOGGER_DEFAULT_PROGRESS_COLOR_CODE,
	GB_LOGGER_DEFAULT_PROGRESS_BAR_COLOR_CODE},
    _log_default_streambuf(std::cout.rdbuf()),
    _error_default_streambuf(std::cerr.rdbuf()),
    _progress_bar_width(GB_LOGGER_DEFAULT_PROGRESS_BAR_WIDTH),
    _progress_total_width(GB_LOGGER_DEFAULT_PROGRESS_TOTAL_WIDTH),
    _bProgressing(false)
{
    _progress_flexible_width = _progress_total_width - (_progress_bar_width + strlen(GB_LOGGER_DEFAULT_PROGRESS_FIXED_CHARS));
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
    assert(!_bProgressing);					\
    assert(szMsg != nullptr);					\
    GB_GET_LOCALTIME(timeBuf);					\
    ::SetConsoleTextAttribute(_hConsole, win_color_code);	\
    ostream << timeBuf << std::endl				\
    << title << szMsg << std::endl;

#elif __GNUC__

#define _gb_fancy_print(ostream, title, color_code)			\
    assert(!_bProgressing);						\
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
    _bProgressing = true;
    
    static std::uint64_t preTime = 0;
    const std::uint64_t curTime = time::Instance().timestamp();
    static const std::uint8_t etaCount = 5;
    static float eta[etaCount] = {0.0f};

    string strEta;
    std::uint8_t widthLeft;
    if(value < 1.0f)
    {
	if(curTime - preTime > 1000)//sample per second
	{
	    static float preValue = 0;
	    if(preValue != 0)
	    {
		const float speed = value - preValue;
		static std::uint8_t eta_idx = 0;
		eta_idx++;
		float& curEta = eta[eta_idx % etaCount];
		if(speed > 0.0f)
		    curEta = (1.0f - value)/speed;
		else
		    curEta = 0.0f;
	    }
	    preValue = value;
	    preTime = curTime;
	}
    
	float average_eta = 0.0f;
	std::uint8_t valid_cout = 0;
	for(int i = 0; i < etaCount; i++)
	{
	    float e = eta[i];
	    if(e > 0.0f)
	    {
		assert((std::numeric_limits<float>::max() - average_eta) > e);
		average_eta += e;

		valid_cout++;
	    }
	}

	if(valid_cout != 0)
	    average_eta = average_eta / valid_cout;
	else
	    average_eta = 0;
	strEta = "ETA: " + time::Instance().format(average_eta);
	widthLeft = _progress_flexible_width - strEta.length() - 3/*anim ...*/;
    }
    else
	widthLeft = _progress_flexible_width;

    std::uint8_t lenTitle;
    if(title != nullptr)
	lenTitle = strlen(title);
    else
	lenTitle = 0;
    assert(widthLeft > lenTitle);
    const std::uint8_t paddingWidth = widthLeft - lenTitle;
//    if(paddingWidth)
    
    const std::string& c0 = _progress_color_code[0];
    const std::string& c1 = _progress_color_code[1];

    std::cout << GB_LOGGER_COLOR_BEGIN << c0 << GB_LOGGER_COLOR_END
	      << ">>>";
    if(title != nullptr)
	std::cout << title;
    std::cout << '['
	      << GB_LOGGER_COLOR_BEGIN << c1 << GB_LOGGER_COLOR_END;
    
    const std::uint8_t width = value * _progress_bar_width;
    for(int i = 0; i < width; i++)
	std::cout << ' ';
    
    std::cout << GB_LOGGER_COLOR_BEGIN << c0 << GB_LOGGER_COLOR_END;
    
    const std::uint8_t width_left = _progress_bar_width - width;
    for(int i = 0; i < width_left; i++)
	std::cout << ' ';

    std::cout << ']' << int(value * 100) << '%' << ' ';
    
    //anim
    static const char anim[6][4] = {{'.', ' ', ' ', '\0'},
				    {' ', '.', ' ', '\0'},
				    {' ', ' ', '.', '\0'},
				    {'.', ' ', '.', '\0'},
				    {' ', '.', '.', '\0'},
				    {'.', '.', '.', '\0'}};
    static unsigned char count = 0;
    count++;

    if(value < 1.0f)
	std::cout << strEta << anim[count % 6];
    for(int i = 0 ; i < paddingWidth; i++)
	std::cout << ' ';
    std::cout << GB_LOGGER_COLOR_BACKTONORMAL << '\r';
    
    std::cout.flush();
}

void logger::progress_done()
{
    std::cout << std::endl;
    _bProgressing = false;
}
void logger::set_progress_width(const std::uint8_t barWidth, const std::uint8_t totalWidth)
{
    _progress_bar_width = barWidth;
    _progress_total_width = totalWidth;
    _progress_flexible_width = _progress_total_width - (_progress_bar_width + strlen(GB_LOGGER_DEFAULT_PROGRESS_FIXED_CHARS));
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
