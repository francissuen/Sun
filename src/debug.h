/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include "common.h"
#include "logger.h"

#ifdef _MSC_VER
#define FS_SUN_FUNC_NAME __FUNCSIG__
#elif defined (__GNUC__)
#define FS_SUN_FUNC_NAME __PRETTY_FUNCTION__
#endif

#ifdef NDEBUG
#define FS_SUN_ASSERT(condition) 
#else
#define FS_SUN_ASSERT(condition)                                        \
    {                                                                   \
        if(!(condition))                                                \
        {                                                               \
            std::string msg;                                            \
            msg += "\n**************** BEGIN FS_SUN_ASSERT FAILED ****************\n"; \
            msg += "@CONDITION: " #condition "\n";                      \
            msg = msg + "@LINE: "  + std::to_string(__LINE__) + "\n";   \
            msg = msg + "@FILE: " + __FILE__ + "\n";                    \
            msg = msg + "@FUNCTION: " + FS_SUN_FUNC_NAME + "\n";        \
            msg += "**************** END OF FS_SUN_ASSERT FAILED ****************\n"; \
            fs::sun::cout("FS_SUN_ASSERT", msg, fs::Sun::logger::S_FATAL); \
            fs::sun::cout.Flush();                                      \
            assert(false);                                              \
        }                                                               \
    }
#endif
