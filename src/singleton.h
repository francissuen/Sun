/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"

FS_SUN_NS_BEGIN

template <typename T>
class Singleton
{
public:
    static T & Instance()
    {
        static T instance;
        return instance;
    }
    Singleton(const Singleton &) = delete;
    Singleton & operator=(const Singleton &) = delete;
protected:
    Singleton() = default;
    ~Singleton() = default;
};

template <typename T>
T & GetSingleton()
{
    static T instance;
    return instance;
}

FS_SUN_NS_END
