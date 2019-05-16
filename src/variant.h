/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include <type_traits>
#include "utils.h"
#include <unordered_map>
#include <typeinfo>

FS_SUN_NS_BEGIN

/**
 *  \brief A safe union, and need no RTTI
 */
template<typename ... Ele_t>
class variant
{
private:
    template<typename T>
    struct _is_valid_type
    {
        static constexpr bool value = ((not std::is_const<T>::value) and (not std::is_reference<T>::value));
    };
    static_assert(static_and<_is_valid_type, Ele_t ...>::value, "not all types in Ele_t are valid type");
public:
    typedef typename std::aligned_storage<static_max<std::size_t, sizeof(Ele_t)...>::value,
                                          static_max<std::size_t, alignof(Ele_t)...>::value>::type data_t;

public:
    static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();
public:
    variant():
        _idx(npos)
    {
        for_each<_store_dtor, Ele_t...>();        
    }

    
public:
    template<typename T, typename ... Args>
    T & emplace(Args && ... args)
    {
        static_assert(is_one_of<T, Ele_t ...>::value, "T is not one of Ele_t");
        _idx = index_of_seq<T, Ele_t ...>::value;
        new (&_data) T(std::forward<Args>(args) ...);
        return reinterpret_cast<T&>(_data);
    }

    template<typename T>
    void operator=(T && t)
    {
        static_assert(is_one_of<T, Ele_t ...>::value, "T is not one of Ele_t");
        _call_dtor();
        new (&_data) T(std::forward<T>(t));
        static constexpr std::size_t idx = index_of_seq<T, Ele_t ...>::value;
        static_assert(idx != index_of_seq<T, Ele_t ...>::npos,
                      "_idx is index_of_seq::npos");
        _idx = idx;
    }

    std::size_t index() const
    {
        return _idx;
    }
    
    template<typename T>
    bool is() const
    {
        static_assert(is_one_of<T, Ele_t ...>::value, "T is not one of Ele_t");
        static constexpr std::size_t idx = index_of_seq<T, Ele_t ...>::value;
        return _idx == idx;
    }

    /**
     *  \warining No guarantee for the right value.
     */
    template<typename T>
    T & raw_get() noexcept
    {
        static_assert(is_one_of<T, Ele_t ...>::value, "T is not one of Ele_t");
        return reinterpret_cast<T&>(_data);
    }

    /**
     *  \warining No guarantee for the right value.
     */
    template<typename T>
    const T & raw_get() const noexcept
    {
        return const_cast<variant*>(this)->raw_get<T>();
    }

    template<typename T>
    T & get()
    {
        static constexpr std::size_t idx = index_of_seq<T, Ele_t ...>::value;
        if(idx != index_of_seq<T, Ele_t ...>::npos)
            return raw_get<T>();
        else
            throw std::bad_cast();
    }
    
    template<typename T>
    const T & get() const
    {
        return const_cast<variant*>(this)->get<T>();
    }

private:
    template<typename T>
    struct _store_dtor
    {
        void operator()(const std::size_t idx) const
        {
            if(std::is_class<T>::value)
                _dtors.insert(std::make_pair(idx, [](void* ptr)
                                             {
                                                 ((T*)ptr)->~T();
                                             }));
        }
    };

    inline void _call_dtor()
    {
        const auto dtor = _dtors.find(_idx);
        if(dtor != _dtors.end())
            dtor->second(&_data);
    }
    
private:
    data_t _data;
    std::size_t _idx;
    static std::unordered_map<std::size_t, std::function<void(void*)>> _dtors;
};

template<typename ... Ele_t>
std::unordered_map<std::size_t, std::function<void(void*)>> variant<Ele_t ...>::_dtors;

FS_SUN_NS_END
