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
template<typename ... Ts>
class Variant
{
private:
    template<typename T>
    struct IsValidType
    {
        static constexpr bool value = ((! std::is_const<T>::value)
                                       && (! std::is_reference<T>::value));
    };
    static_assert(StaticAnd<IsValidType, Ts ...>::value, "not all types in Ts are valid type");

    typedef typename std::aligned_storage<StaticMax<std::size_t, sizeof(Ts)...>::value,
                                          StaticMax<std::size_t, alignof(Ts)...>::value
                                          >::type RawData;

    template<typename T>
    struct StoreDtor
    {
        void operator()() const
        {
            if(std::is_class<T>::value)
                dtors_.insert(std::make_pair(IndexOf<T>::template In<Ts ...>::value, [](void* ptr)
                                             {
                                                 (reinterpret_cast<T*>(ptr))->~T();
                                             }));
        }
    };

public:
#ifdef _MSC_VER
#undef max
#endif
    static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();
public:
    Variant():
        idx_(npos)
    {
        Invoke<StoreDtor>::template For<Ts...>::With();
    }

    
public:
    template<typename T, typename ... TArgs>
    T & Emplace(TArgs && ... args)
    {
        static_assert(IsType<T>::template In<Ts ...>::value, "T is not one of Ts");
        idx_ = IndexOf<T>::template In<T, Ts ...>::value;
        new (&raw_data_) T(std::forward<TArgs>(args) ...);
        return reinterpret_cast<T&>(raw_data_);
    }

    template<typename T>
    void operator=(T && t)
    {
        static_assert(IsType<T>::template In<Ts ...>::value, "T is not one of Ts");
        CallDtor();
        new (&raw_data_) T(std::forward<T>(t));
        static constexpr std::size_t idx = IndexOf<T>::template In<Ts ...>::value;
        static_assert(idx != IndexOf<T>::npos,
                      "idx is npos");
        idx_ = idx;
    }

    std::size_t Index() const
    {
        return idx_;
    }
    
    template<typename T>
    bool Is() const
    {
        static_assert(IsType<T>::template In<Ts ...>::value, "T is not one of Ts");
        static constexpr std::size_t idx = IndexOf<T>::template In<Ts ...>::value;
        return idx_ == idx;
    }

    /**
     *  \warining No guarantee for the right value.
     */
    template<typename T>
    T & RawGet() noexcept
    {
        static_assert(IsType<T>::template In<Ts ...>::value, "T is not one of Ts");
        return reinterpret_cast<T&>(raw_data_);
    }

    /**
     *  \warining No guarantee for the right value.
     */
    template<typename T>
    const T & RawGet() const noexcept
    {
        return const_cast<Variant*>(this)->RawGet<T>();
    }

    template<typename T>
    T & Get()
    {
        if(Is<T>())
            return RawGet<T>();
        else
            throw std::bad_cast();
    }
    
    template<typename T>
    const T & Get() const
    {
        return const_cast<Variant*>(this)->Get<T>();
    }

private:
    inline void CallDtor()
    {
        const auto dtor = dtors_.find(idx_);
        if(dtor != dtors_.end())
            dtor->second(&raw_data_);
    }
    
private:
    RawData raw_data_;
    std::size_t idx_;
    static std::unordered_map<std::size_t, std::function<void(void*)>> dtors_;
};

template<typename ... Ts>
std::unordered_map<std::size_t, std::function<void(void*)>> Variant<Ts ...>::dtors_;

FS_SUN_NS_END
