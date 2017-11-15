#include "time.h"

//SingletonDefine(gbTime)
using gb::utils::time;
void time::get_local_time(char * const buffer, const unsigned int length)
{
	if (length > MAXTIMEBUFFERLEN)
		return;
	time_t rawTime;
	::time(&rawTime);

	tm* pTime = localtime(&rawTime);
	sprintf(buffer, "%d:%d:%d(%d,%d,%d)", pTime->tm_hour, pTime->tm_min, pTime->tm_sec, (pTime->tm_mon + 1), pTime->tm_mday, 1900+pTime->tm_year);
}

int64_t time::get_local_time()
{
	time_t rawTime;
	::time(&rawTime);
	return rawTime;
}

void time::format_time(int64_t rawTime, char * const buffer, const unsigned int length)
{
	int mins = rawTime / 60;
	int hours = mins / 60;
	int days = hours / 24;

	int hs = hours % 24;
	int ms = mins % 60;
	int ss = rawTime % 60;
	sprintf(buffer, "%ddays,%dhours, %dminutes, %dseconds", days, hs, ms, ss);
}
