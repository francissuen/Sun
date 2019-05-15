/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include <unordered_map>
#include <functional>
#include <memory>
#include "logger.h"

template<typename base_t, typename goods_tag_t, typename ... args_t>
class factory
{
public:
    using ctor_t = std::function<std::unique_ptr<base_t>(args_t ...)>>;
public:
    
    void add(const goods_tag_t & tag, const ctor_t & ctor)
    {
        _ctors.insert(std::make_pair(tag, ctor));
    }

    void remove(const goods_tag_t & tag)
    {
        _ctors.erase(tag);
    }
    
    std::unique_ptr<base_t> create(const goods_tag_t & tag, args_t ... args)
    {
        const auto & itr = _ctors.find(tag);
        if(itr != _ctors.end())
            return itr->second(args ...);
        else
        {
            cout("No corresponding ctor found @tag: " + std::to_string(tag), logger::S_ERROR);
            return nullptr;
        }
    }

private:
    std::unordered_map<goods_tag_t, ctor_t> _ctors;
};
