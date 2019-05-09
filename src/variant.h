/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include <type_traits>
#include "utils.h"
#include <unordered_map>

FS_SUN_NS_BEGIN

template<typename ... Ele_t>
class variant
{
public:
    typedef typename std::aligned_storage<static_max<std::size_t, sizeof(Ele_t)...>::value,
                                          static_max<std::size_t, alignof(Ele_t)...>::value>::type data_t;

public:
    static constexpr std::uint8_t npos = std::numeric_limits<std::uint8_t>::max();
public:
    variant():
        _idx(npos)
    {
        for_each<_store_dtor, Ele_t...>();        
    }
    
    /** template<typename T, typename ... Args> */
    /** variant(Args ... args): */
    /**     _idx(index_of_seq<T, Ele_t ...>::value) */
    /** { */
    /**     new (&_data) T(args...); */
    /**     for_each<_store_dtor, Ele_t...>(); */
    /** } */
    
public:
    template<typename T>
    void operator=(T && t)
    {
        _call_dtor();
        new (&_data) T(std::forward<T>(t));
        _idx = index_of_seq<T, Ele_t ...>::value;
    }

    std::uint8_t index() const
    {
        return _idx;
    }
private:
    template<typename T>
    struct _store_dtor
    {
        void operator()(const std::uint8_t idx) const
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
    std::uint8_t _idx;
    static std::unordered_map<std::uint8_t, std::function<void(void*)>> _dtors;
};

template<typename ... Ele_t>
std::unordered_map<std::uint8_t, std::function<void(void*)>> variant<Ele_t ...>::_dtors;

FS_SUN_NS_END
