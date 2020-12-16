/* Copyright (C) 2020 Francis Sun, all rights reserved. */

#ifndef FS_SUN_OS_H
#define FS_SUN_OS_H

#ifdef __linux__
#define FS_SUN_OS_LINUX
#elif defined(_WIN32)
#define FS_SUN_OS_WINDOWS
#elif defined(__APPLE__)
#define FS_SUN_OS_APPLE
#else
#error unsupported platform
#endif

#endif  // FS_SUN_OS_H
