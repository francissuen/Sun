/* Copyright (C) 2021 Francis Sun, all rights reserved. */

#ifndef FS_SUN_ASSERT_H
#define FS_SUN_ASSERT_H

#include <cassert>

/*
 * \brief Return ... if !(expr), no matter ifdef NDEBUG.
 */
#define FS_SUN_ASSRET(expr, ...) \
  if (!(expr)) {                 \
    assert(false);               \
    return __VA_ARGS__;          \
  }

#ifdef FS_SUN_NO_NS
#define ASSRET(expr, ...) FS_SUN_ASSRET(expr, __VA_ARGS__)
#endif

#endif  // FS_SUN_ASSERT_H
