/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_PLATFORM_H
#define FS_SUN_PLATFORM_H

#ifdef __linux__
#define FS_SUN_LINUX
#elif defined(_WIN32)
#define FS_SUN_WINDOWS
#elif defined(__APPLE__)
#define FS_SUN_APPLE
#else
#error unsupported platform
#endif

#endif  // FS_SUN_PLATFORM_H
