/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include <memory>
#include "debug.h"

FS_SUN_NS_BEGIN

template<typename T>
class RangeViewer
{
public:
    RangeViewer(){}
    
    RangeViewer(const std::size_t size):
        data_(new T[size]{}, std::default_delete<T[]>{}),
        max_size_(size),
        begin_(0),
        end_(size)
    {}

public:
    RangeViewer SubViewer(const std::ptrdiff_t begin_offset, const std::size_t new_size)
    {
        RangeViewer ret(*this);
        ret.advancebegin_(begin_offset);
        ret.setend_(ret.begin_ + new_size);
        return ret;
    }
    
    RangeViewer SubViewer(const std::size_t begin_offset)
    {
        RangeViewer ret(*this);
        ret.advancebegin_(begin_offset);
        return ret;
    }

    std::size_t Size() const
    {
        return end_ - begin_;
    }

    template<typename U>
    const U & Get(std::size_t index = 0) const
    {
        FS_SUN_ASSERT(begin_ + index * sizeof(U) < end_);
        return *(reinterpret_cast<U*>(data_.get() + begin_) + index);
    }

    template<typename U>
    void Set(const U & value, std::size_t index = 0) const
    {
        FS_SUN_ASSERT(begin_ + index * sizeof(U) < end_);
        *(reinterpret_cast<U*>(data_.get() + begin_) + index) = value;
    }
    
    std::shared_ptr<T> Data()
    {
        return std::shared_ptr<T>(data_, data_.get() + begin_);
    }
    
    const std::shared_ptr<T> & Data() const
    {
        return const_cast<RangeViewer*>(this)->Data();
    }

    void SetBegin(const std::size_t new_begin)
    {
        begin_ = new_begin;
        FS_SUN_ASSERT(begin_ <= end_);
    }
    void AdvanceBegin(const std::ptrdiff_t n)
    {
        begin_ += n;
        FS_SUN_ASSERT(begin_ <= end_);
    }
    
    void SetEnd(const std::size_t new_end)
    {
        end_ = new_end;
        FS_SUN_ASSERT(begin_ <= end_ && end_ <= max_size_);
    }

    void AdvanceEnd(const std::ptrdiff_t n)
    {
        end_ += n;
        FS_SUN_ASSERT(begin_ <= end_ && end_ <= max_size_);
    }
    
private:
    std::shared_ptr<T> data_;
    std::size_t max_size_;
    std::size_t begin_;
    std::size_t end_;
};

FS_SUN_NS_END
