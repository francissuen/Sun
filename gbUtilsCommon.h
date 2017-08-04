#pragma once
#include "gbCommon.h"

#define gbUtilsDLLEXPORT 1

#ifdef gbUtilsDLLEXPORT
#define gbUtilsDLL_CLASS class _declspec(dllexport)
#else
#define gbUtilsDLL_CLASS class _declspec(dllimport)
#endif

#ifdef gbUtilsDLLEXPORT
#define gbUtilsDLL_STRUCT struct _declspec(dllexport)
#else
#define gbUtilsDLL_STRUCT struct _declspec(dllimport)
#endif


