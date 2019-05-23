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
        _data(new T[size]{}, std::default_delete<T[]>{}),
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

    template<typename U>
    const U & get(std::size_t index = 0) const
    {
        FS_SUN_ASSERT(_begin + index * sizeof(U) < _end);
        return *(reinterpret_cast<U*>(_data.get() + _begin) + index);
    }

    template<typename U>
    void set(const U & value, std::size_t index = 0)
    {
        FS_SUN_ASSERT(_begin + index * sizeof(U) < _end);
        *(reinterpret_cast<U*>(_data.get() + _begin) + index) = value;
    }
    
    std::shared_ptr<T> data()
    {
        return std::shared_ptr<T>(_data, _data.get() + _begin);
    }
    
    const std::shared_ptr<T> & data() const
    {
        return const_cast<range_viewer*>(this)->data();
    }

    void advance_begin(const std::ptrdiff_t n)
    {
        _begin += n;
        FS_SUN_ASSERT(_begin < _end);
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
