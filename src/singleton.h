/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"

FS_SUN_NS_BEGIN

template <typename T>
class singleton
{
public:
    static T & instance()
    {
        static T instance;
        return instance;
    }
    singleton(const singleton &) = delete;
    singleton & operator=(const singleton &) = delete;
protected:
    singleton() = default;
    ~singleton() = default;
};

template <typename T>
T & get_singleton()
{
    static T instance;
    return instance;
}

FS_SUN_NS_END
