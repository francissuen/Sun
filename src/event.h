/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include <unordered_map>

FS_SUN_NS_BEGIN

template<typename TEvent, typename TCallback, typename TCallbackTag = std::uintptr_t>
class Event
{
public:
    void Add(const TEvent & ev, const TCallbackTag & cb_tag, const TCallback & cb)
    {
        const auto & itr = callbacks_.find(ev);
        if(itr != callbacks_.end())
        {
            itr->second.insert(std::make_pair(cb_tag, cb));
        }
        else
            callbacks_.insert(std::make_pair(ev, {cb_tag, cb}));
    }
    
    void Remove(const TEvent & ev, const TCallbackTag & cb_tag)
    {
        const auto & itr = callbacks_.find(ev);
        if(itr != callbacks_.end())
        {
            auto & cbs = itr->second;
            const auto & cb_itr = cbs.find(cb_tag);
            if(cb_itr != cbs.end())
                cbs.erase(cb_itr);
        }
    }

    template<typename ... TArgs>
    void Trigger(const TEvent & ev, TArgs ... args)
    {
        const auto & itr = callbacks_.find(ev);
        if(itr != callbacks_.end())
        {
            for(const auto & cb : itr->second)
            {
                cb->second(args ...);
            }
        }
    }

    
private:
    std::unordered_map<TEvent, std::unordered_map<TCallbackTag, TCallback>> callbacks_;
};

FS_SUN_NS_END
