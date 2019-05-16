/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once
#include "ns.h"
#include <unordered_map>
#include <functional>
#include <memory>
#include "logger.h"

FS_SUN_NS_BEGIN

using factory_order_num_t = std::size_t;

template<typename base_t, typename ... ctor_args_t>
class factory
{
public:
    using order_num_t = factory_order_num_t;
    using goods_base_t = base_t;
    using ctor_t = std::function<std::unique_ptr<goods_base_t>(ctor_args_t ...)>;
    template<typename ... goods_t>
    friend class take_order;

    template<typename ... goods_t>
    class with
    {
    public:
        operator factory<base_t, ctor_args_t ...>()
        {
            factory<base_t, ctor_args_t ...> f;
            f.template construct<goods_t ...>();
            return f;
        }

        template<typename T>
        static constexpr order_num_t order_num_of()
        {
            return index_of_seq<T, goods_t ...>::value;
        }
    };

private:
    template<typename T, typename ... goods_t>
    struct _construct
    {
        operator std::pair<const order_num_t, ctor_t>()
        {
            /** non-odr-used version */
            static constexpr order_num_t order_num = index_of_seq<T, goods_t ...>::value;
            return {order_num, [](ctor_args_t ... args) -> std::unique_ptr<base_t>{
                    return std::unique_ptr<base_t>(new T(args ...));}};
        }
    };

public:
    std::unique_ptr<goods_base_t> create(const order_num_t order_num, ctor_args_t ... args) const
    {
        const auto & itr = _ctors.find(order_num);
        if(itr != _ctors.end())
            return itr->second(args ...);
        else
        {
            cout("No corresponding ctor found @order_num: " + std::to_string(order_num), logger::S_ERROR);
            return nullptr;
        }
    }

private:
    template <typename ... goods_t>
    void construct()
    {
        _ctors = std::unordered_map<order_num_t, ctor_t>{_construct<goods_t, goods_t ... >() ...};
    }

private:
    std::unordered_map<order_num_t, ctor_t> _ctors;
};


/**
 * \brief A static type version factory.
 */
template<typename base_t, typename ... goods_t>
class static_factory
{
public:
    using order_num_t = factory_order_num_t;
    using goods_base_t = base_t;


    template<typename ... ctor_args_t>
    class with_ctor_t
    {
    public:
        using ctor_t = std::function<std::unique_ptr<goods_base_t>(ctor_args_t ...)>;
    private:
        template<typename T>
        struct _ctor_pair
        {
            operator std::pair<const order_num_t, ctor_t>()
            {
                /** non-odr-used version */
                static constexpr order_num_t order_num = index_of_seq<T, goods_t ...>::value;
                return {order_num, [](ctor_args_t ... args) -> std::unique_ptr<base_t>{
                        return std::unique_ptr<base_t>(new T(args ...));}};
            }
        };

    public:
        with_ctor_t():
            _ctors{_ctor_pair<goods_t>()...}
        {}
    public:
        std::unique_ptr<base_t> create(const order_num_t order_num, ctor_args_t ... args) const
        {
            const auto & itr = _ctors.find(order_num);
            if(itr != _ctors.end())
                return itr->second(args ...);
            else
            {
                cout("No corresponding ctor found @order_num: " + std::to_string(order_num), logger::S_ERROR);
                return nullptr;
            }
        }
    
    private:
        const std::unordered_map<order_num_t, ctor_t> _ctors;
    };
    
public:
    template<typename T>
    constexpr static order_num_t order_num_of()
    {
        return index_of_seq<T, goods_t ...>::value;
    }

};

FS_SUN_NS_END
