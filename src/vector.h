#pragma once

#include "ns.h"
#include <cstring>

GB_UTILS_NS_BEGIN

template <typename T>
class vector
{
public:
    vector():
	_capacity(0),
	_count(0),
	_data(nullptr)
	{}
    vector(const std::size_t capacity):
	_capacity(capacity),
	_count(0)
	{
	    _data = new T[_capacity];
	}
    vector(const vector & o):
	_capacity(o._capacity),
	_count(o._count)
	{
	    _data = new T[_capacity];
	    std::memcpy(_data, o._data, sizeof(T) * _count);
	}
    vector( vector && o):
	_capacity(o._capacity),
	_count(o._count),
	_data(o._data)
	{
	    o._capacity = 0;
	    o._count = 0;
	    o._data = 0;
	}
    
    ~vector()
	{
	    delete _data;
	    _data = nullptr;
	}

public:
    void reserve(const std::size_t capacity)
	{
	    _count = 0;
	    if(_capacity != capacity)
	    {
		_capacity = capacity;
		delete _data;
		_data = new T[_capacity];
	    }
	}
    template <typename TR>
    void push_back( TR && val )
	{
	    const std::size_t oldCount = _count;
	    _count += 1;
	    
	    if(_count <= _capacity)
		_data[oldCount] = std::forward<TR>(val);
	    else
	    {
		_capacity *= 2;
		T * newData = new T[_capacity];
		// old data
		for(std::size_t idx = 0; idx < oldCount; idx ++)
		{
		    newData[idx] = std::move(_data[idx]);
		}

		// new data
		_data[oldCount] = std::forward<TR>(val);
	    }
	}

    const T & operator[](const std::size_t idx) const
	{
	    assert(idx < _count);
	    return _data[idx];
	}

    T & operator[](const std::size_t idx)
	{
	    assert(idx < _count);
	    return _data[idx];
	}

    const T* data() const
	{
	    return _data;
	}

    T* move_data()
	{
	    T * const data = _data;
	    _data = nullptr;
	    _capacity  = 0;
	    _count = 0;
	    
	    return data;
	}

    std::size_t count() const
	{
	    return _count;
	}
    std::size_t capacity() const
	{
	    return _capacity;
	}
private:
    std::size_t _capacity;
    std::size_t _count;
    T * _data;
};

GB_UTILS_NS_END
