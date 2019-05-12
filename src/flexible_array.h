/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include <vector>
#include "debug.h"

FS_SUN_NS_BEGIN

template<typename T>
class flexible_array
{
public:
    using data_t = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
public:
    flexible_array(const std::size_t capacity):
        _data(new data_t[capacity]),
        _capacity(capacity),
        _beginItr(0),
        _endItr(capacity - 1)
    {}
public:
    std::size_t size() const
    {
        return _endItr - _beginItr;
    }
    T * data()
    {
        return reinterpret_cast<T*>(_data) + _beginItr;
    }
    const T * data() const
    {
        return const_cast<flexible_array*>(this)->data();
    }
    void set_begin(const std::size_t new_begin)
    {
        FS_SUN_ASSERT(new_begin < _capacity);
        _beginItr = new_begin;
    }
    void begin_increase(const std::size_t begin_offset)
    {
        _beginItr += begin_offset;
        FS_SUN_ASSERT(_beginItr < _capacity);
    }

    void set_end(const std::size_t new_end)
    {
        FS_SUN_ASSERT(new_end < _capacity && new_end > _beginItr);
        _endItr = new_end;
    }

private:
    data_t * _data;
    std::size_t _capacity, _beginItr, _endItr;
};

FS_SUN_NS_END
