/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include "singleton.h"
#include "string.h"

FS_SUN_NS_BEGIN

FS_SUN_CLASS Time : public Singleton<Time>
{
    friend class Singleton<Time>;
private:
    Time() = default;
public:
    std::string LocalTime() const;
    std::uint64_t Timestamp()const;
};

FS_SUN_NS_END


