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
    range_viewer(){}
    
    range_viewer(const std::size_t size):
        _data(new T[size]{}, std::default_delete<T[]>{}),
        _max_size(size),
        _begin(0),
        _end(size)
    {}

public:
    range_viewer sub_viewer(const std::ptrdiff_t begin_offset, const std::size_t new_size)
    {
        range_viewer ret(*this);
        ret.advance_begin(begin_offset);
        ret.set_end(ret._begin + new_size);
        return ret;
    }
    
    range_viewer sub_viewer(const std::size_t begin_offset)
    {
        range_viewer ret(*this);
        ret.advance_begin(begin_offset);
        return ret;
    }

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
    void set(const U & value, std::size_t index = 0) const
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

    void set_begin(const std::size_t new_begin)
    {
        _begin = new_begin;
        FS_SUN_ASSERT(_begin <= _end);
    }
    void advance_begin(const std::ptrdiff_t n)
    {
        _begin += n;
        FS_SUN_ASSERT(_begin <= _end);
    }
    
    void set_end(const std::size_t new_end)
    {
        _end = new_end;
        FS_SUN_ASSERT(_begin <= _end && _end <= _max_size);
    }

    void advance_end(const std::ptrdiff_t n)
    {
        _end += n;
        FS_SUN_ASSERT(_begin <= _end && _end <= _max_size);
    }
    
private:
    std::shared_ptr<T> _data;
    std::size_t _max_size;
    std::size_t _begin;
    std::size_t _end;
};

FS_SUN_NS_END
