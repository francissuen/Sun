/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include "singleton.h"
#include "string.h"

FS_SUN_NS_BEGIN

FS_SUN_CLASS time : public singleton<time>
{
    friend class singleton<time>;
private:
    time() = default;
public:
    std::string localtime() const;
    std::uint64_t timestamp()const;
};

FS_SUN_NS_END


