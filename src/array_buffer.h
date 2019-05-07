/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include <array>
#include "debug.h"

FS_SUN_NS_BEGIN

template<typename T, std::size_t N>
class array_buffer
{
public:
    std::size_t size() const
    {
        return _size;
    }
    inline T & operator[](const std::size_t idx)
    {
        FS_SUN_ASSERT(idx < _size);
        return _data[idx];
    }
    inline const T & operator[](const std::size_t idx) const
    {
        FS_SUN_ASSERT(idx < _size);
        return _data[idx];
    }
    void resize(const std::size_t new_size)
    {
        FS_SUN_ASSERT(new_size <= N);
        _size = new_size;
    }
    template<typename ele_t>
    void push_back(ele_t && ele)
    {
        FS_SUN_ASSERT(_size < N);
        _data[_size++] = std::forward<ele_t>(ele);
    }
    T * data()
    {
        return _data.data();
    }
    const T * data() const
    {
        return _data.data();
    }
private:
    std::array<T, N> _data;
    std::size_t _size;
};

FS_SUN_NS_END
