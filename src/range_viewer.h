/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include <memory>
#include "debug.h"

FS_SUN_NS_BEGIN

template<typename T>
class range_viewer
{
public:
    range_viewer(const std::size_t size):
        _data(new T[size]{}),
        _max_size(size),
        _begin(0),
        _end(size)
    {}

    range_viewer(const std::shared_ptr<T> & data, const std::size_t size):
        _data(data),
        _max_size(size),
        _begin(0),
        _end(size)
    {}
    
public:
    std::size_t size() const
    {
        return _end - _begin;
    }
    
    T* data()
    {
        return _data.get() + _begin;
    }
    
    const T* data() const
    {
        return const_cast<range_viewer*>(this)->data();
    }

    void advance_begin(const std::ptrdiff_t n)
    {
        _begin += n;
        FS_SUN_ASSERT(_begin < _end && _begin < _max_size);
    }
    
    void set_end(const std::size_t new_end)
    {
        _end = new_end;
        FS_SUN_ASSERT(_end > _begin && _end <= _max_size);
    }
private:
    std::shared_ptr<T> _data;
    const std::size_t _max_size;
    std::size_t _begin;
    std::size_t _end;
};

FS_SUN_NS_END
