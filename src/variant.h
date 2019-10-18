/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include <type_traits>
#include "utility.h"
#include <unordered_map>
#include <typeinfo>
#include "string.h"
#include <cstring>

#include "debug.h"


FS_SUN_NS_BEGIN

template<typename ... Ts>
class Variant;

namespace variant
{
    
#ifdef _MSC_VER
#ifdef max
#undef max
#endif
#endif
    using index_t = std::uint8_t;
    static constexpr index_t npos = std::numeric_limits<index_t>::max();
    
    template<typename T>
    struct CopyCtor
    {
        template<typename ... Ts>
        void operator()(Variant<Ts...> * dst, const Variant<Ts...> & src);
    };

    template<typename T>
    struct MoveCtor
    {
        template<typename ... Ts>
        void operator()(Variant<Ts...> * dst, Variant<Ts...> && src);
    };


    template<typename T>
    struct CopyAssignmentOperator
    {
        template<typename ... Ts>
        void operator()(Variant<Ts...> * dst, const Variant<Ts...> & src);
    };

    template<typename T>
    struct MoveAssignmentOperator
    {
        template<typename ... Ts>
        void operator()(Variant<Ts...> * dst, Variant<Ts...> && src);
    };


    template<typename T>
    struct Dtor
    {
        template<typename ... Ts>
        void operator()(Variant<Ts...> * dst);
    };

    template<typename T>
    struct Swap
    {
        template<typename ... Ts>
        void operator()(Variant<Ts...> * a, Variant<Ts...> & b);
    };

    template<typename T>
    struct ToString
    {
        template<typename ... Ts>
        void operator()(const Variant<Ts...> * src, std::string & ret);
    };
}
/**
 *  \brief A safe union, and needs no RTTI
 */
template<typename ... Ts>
class Variant
{
    static_assert(sizeof...(Ts) < variant::npos, "Too many Ts for variant::index_t.");
    
    template<typename T>
    friend struct variant::CopyCtor;

    template<typename T>
    friend struct variant::MoveCtor;

    template<typename T>
    friend struct variant::CopyAssignmentOperator;

    template<typename T>
    friend struct variant::MoveAssignmentOperator;

    template<typename T>
    friend struct variant::Dtor;

    template<typename T>
    friend struct variant::Swap;

    template<typename T>
    friend struct variant::ToString;
    
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

public:
    Variant():
        raw_data_{},
        index_{variant::npos}
    {}

    Variant(const Variant & other)
    {
        CallMemberFunction<variant::CopyCtor>(std::forward<Variant>(other));
    }


    Variant(Variant && other)
    {
        CallMemberFunction<variant::MoveCtor>(std::forward<Variant>(other));
    }

    template<typename T>
    Variant(T && other):
        index_(IndexOf<typename remove_cvref<T>::type>::template In<Ts...>::value)
    {
        static_assert(IsType<typename remove_cvref<T>::type>:: template In<Ts...>::value, "Invalid type T.");
        new (&(raw_data_)) typename remove_cvref<T>::type (std::forward<T>(other));
    }

    ~Variant()
    {
        CallMemberFunction<variant::Dtor>();
    }

    
public:

    friend void swap(Variant & a, Variant & b)
    {
        using std::swap;

        /** swap(a.raw_data_, b.raw_data_);*/
        RawData tmp;
        std::memcpy(&tmp, &a.raw_data_, sizeof(a.raw_data_));
        std::memcpy(&a.raw_data_, &b.raw_data_, sizeof(a.raw_data_));
        swap(a.index_, b.index_);
    }

    template<typename T, typename ... TArgs>
    T & Emplace(TArgs && ... args)
    {
        static_assert(IsType<T>::template In<Ts ...>::value, "T is not one of Ts");
        index_ = IndexOf<T>::template In<Ts ...>::value;
        CallMemberFunction<variant::Dtor>();
        
        new (&raw_data_) T(std::forward<TArgs>(args) ...);
        return reinterpret_cast<T&>(raw_data_);
    }

    Variant& operator=(Variant other)
    {
        /** using std::swap; */
        
        /** swap(*this, other); */

        Invoke<variant::CopyAssignmentOperator>::template For<Ts...>::With(this, other);
        
        return *this;
    }

    template<typename T>
    Variant& operator=(T && other)
    {
        static_assert(IsType<typename remove_cvref<T>::type>::template In<Ts...>::value,
                      "T is not one of Ts");
        using std::swap;
        
        {
            Variant tmp(std::forward<T>(other));

            /** Invoke<variant::Swap>::template For<Ts...>::With(this, tmp); */
            FS_SUN_LOG_INFO("this: " + ToString() + ", tmp: " + tmp.ToString());
            FS_SUN_LOG_INFO("tmp index: " + string::ToString(tmp.index_));
            swap(*this, tmp);
            FS_SUN_LOG_INFO("this: " + ToString() + ", tmp: " + tmp.ToString());
            FS_SUN_LOG_INFO("tmp index: " + string::ToString(tmp.index_));
            FS_SUN_LOG_INFO("this index_: " + string::ToString(index_));
        }

        FS_SUN_LOG_INFO("this: " + ToString() + ", index_: " + string::ToString(index_));

        return *this;
    }

    variant::index_t Index() const
    {
        return index_;
    }
    
    template<typename T>
    bool Is() const
    {
        static_assert(IsType<T>::template In<Ts ...>::value, "T is not one of Ts");
        static constexpr variant::index_t idx = IndexOf<T>::template In<Ts ...>::value;
        return index_ == idx;
    }

    /**
     *  \warining No guarantee for a correct value.
     */
    template<typename T>
    T & RawGet() noexcept
    {
        static_assert(IsType<T>::template In<Ts ...>::value, "T is not one of Ts");
        return reinterpret_cast<T&>(raw_data_);
    }

    /**
     *  \warining No guarantee for a correct value.
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
    T && Get() &&
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

    std::string ToString() const
    {
        std::string ret;
        CallMemberFunction<variant::ToString>(ret);
        return ret;
    }
    
private:
    template<template<typename> class TFunctor, typename ... TArgs>
    void CallMemberFunction(TArgs && ... args)
    {
        Invoke<TFunctor>::template For<Ts ...>::With(this, std::forward<TArgs>(args)...);
    }

    template<template<typename> class TFunctor, typename ... TArgs>
    void CallMemberFunction(TArgs && ... args) const
    {
        Invoke<TFunctor>::template For<Ts ...>::With(this, std::forward<TArgs>(args)...);
    }

private:
    RawData raw_data_{};
    variant::index_t index_{variant::npos};
};

namespace variant
{
    template<typename T>
    template<typename ... Ts>
    void CopyCtor<T>::operator()(Variant<Ts...> * dst, const Variant<Ts...> & src)
    {
        if(src.index_ == IndexOf<T>::template In<Ts...>::value)
        {
            new(&(dst->raw_data_))T(src->template Get<T>());
            dst->index_ = src.index_;
        }
    }

    template<typename T>
    template<typename ... Ts>
    void MoveCtor<T>::operator()(Variant<Ts...> * dst, Variant<Ts...> && src)
    {
        if(src.index_ == IndexOf<T>::template In<Ts...>::value)
        {
            new(&(dst->raw_data_))T(std::move(src)->template Get<T>());
            dst->index_ = src.index_;
        }
    }

    template<typename T>
    template<typename ... Ts>
    void CopyAssignmentOperator<T>::operator()(Variant<Ts...> * dst, const Variant<Ts...> & src)
    {
        FS_SUN_LOG_INFO(typeid(T).name());
        if(src.index_ == IndexOf<T>::template In<Ts...>::value)
        {
            FS_SUN_LOG_INFO("src.index_: " + fs::sun::string::ToString(src.index_) + ", dst.index_: " +
                            fs::sun::string::ToString(dst->index_));
            dst->~Variant<Ts...>();
            
            *dst = src.template Get<T>();
        }
    }

    template<typename T>
    template<typename ... Ts>
    void MoveAssignmentOperator<T>::operator()(Variant<Ts...> * dst, Variant<Ts...> && src)
    {
        FS_SUN_LOG_INFO(typeid(T).name());
        if(src.index_ == IndexOf<T>::template In<Ts...>::value)
        {
            FS_SUN_LOG_INFO("src.index_: " + fs::sun::string::ToString(src.index_) + ", dst.index_: " +
                            fs::sun::string::ToString(dst->index_));
            dst->~Variant<Ts...>();
            
            *dst = std::move(src).template Get<T>();
        }
    }

    template<typename T>
    template<typename ... Ts>
    void Dtor<T>::operator()(Variant<Ts...> *dst)
    {
        if(std::is_class<T>::value && dst->index_ == IndexOf<T>::template In<Ts...>::value)
        {
            FS_SUN_LOG_INFO("dtor: " + string::ToString(dst->index_));
            reinterpret_cast<T*>(&(dst->raw_data_))->~T();
            dst->index_ = npos;
        }
    }

    template<typename T>
    template<typename ... Ts>
    void Swap<T>::operator()(Variant<Ts...> * a, Variant<Ts...> & b)
    {
        if(a->index_ == IndexOf<T>::template In<Ts...>::value)
        {
            T tmp = a->template Get<T>();
            Invoke<CopyAssignmentOperator>::template For<Ts...>::With(a, b);
            b = tmp;
        }
    }

    template<typename T>
    template<typename ... Ts>
    void ToString<T>::operator()(const Variant<Ts...> * src, std::string & ret)
    {
        if(src->index_ == IndexOf<T>::template In<Ts...>::value)
        {
            ret = string::ToString(src->template Get<T>());
        }
    }

}

FS_SUN_NS_END
