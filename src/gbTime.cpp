#include "gbTime.h"

//SingletonDefine(gbTime)

void gbTime::GetLocalTime(char * const buffer, const unsigned int length)
{
	if (length > MAXTIMEBUFFERLEN)
		return;
	time_t rawTime;
	time(&rawTime);

	tm* pTime = localtime(&rawTime);
	sprintf(buffer, "%d:%d:%d(%d,%d,%d)", pTime->tm_hour, pTime->tm_min, pTime->tm_sec, (pTime->tm_mon + 1), pTime->tm_mday, 1900+pTime->tm_year);
}

int64_t gbTime::GetLocalTime()
{
	time_t rawTime;
	time(&rawTime);
	return rawTime;
}

void gbTime::FormatTime(int64_t rawTime, char * const buffer, const unsigned int length)
{
	int mins = rawTime / 60;
	int hours = mins / 60;
	int days = hours / 24;

	int hs = hours % 24;
	int ms = mins % 60;
	int ss = rawTime % 60;
	sprintf(buffer, "%ddays,%dhours, %dminutes, %dseconds", days, hs, ms, ss);
}
