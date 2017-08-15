#include <stdio.h>
#include <time.h>
#ifdef __GNUC__
#include<stdint.h>
#endif
#include "gbCommon.h"
#include "gbUtilsConfig.h"

#define MAXTIMEBUFFERLEN 64

#ifdef _MSC_VER
typedef __int64 int64_t;
#endif
gbUtilsEXPORT_CLASS gbTime
{
	SingletonDeclare(gbTime)
public:
	void GetLocalTime(char * const buffer, const unsigned int length = MAXTIMEBUFFERLEN);
  int64_t GetLocalTime();
	void FormatTime(int64_t rawTime, char * const buffer, const unsigned int length = MAXTIMEBUFFERLEN);
};
#define gbLocalTime(x) \
char x[MAXTIMEBUFFERLEN] = { 0 }; \
gbTime::Instance().GetLocalTime(x);

#define gbTimeFormat(rawTime, buffer)\
char buffer[MAXTIMEBUFFERLEN] = { 0 }; \
gbTime::Instance().FormatTime(rawTime, buffer);


