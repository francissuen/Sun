/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include "common.h"
#include "config.h"
#include "string.h"

FS_SUN_NS_BEGIN

FS_SUN_CLASS time : public singleton<time>
{
    friend class singleton<time>;
private:
    time() = default;
public:
    string localtime() const;
    std::uint64_t timestamp()const;

    /**
     * \brief Format seconds to [*d:][*h:][*m:]*s string
     */
    string format(const time_t seconds);
};

FS_SUN_NS_END


