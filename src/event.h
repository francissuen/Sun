/*
 * Copyright (c) 2019, F.S.. All rights reserved.
 */

#pragma once

#include "ns.h"
#include <unordered_map>

FS_SUN_NS_BEGIN

template<typename event_t, typename callback_t, typename callback_tag_t = std::uintptr_t>
class event
{
public:
    void add(const event_t & ev, const callback_tag_t & cb_tag, const callback_t & cb)
    {
        const auto & itr = _callbacks.find(ev);
        if(itr != _callbacks.end())
        {
            itr->second.insert(std::make_pair(cb_tag, cb));
        }
        else
            _callbacks.insert(std::make_pair(ev, {cb_tag, cb}));
    }
    
    void remove(const event_t & ev, const callback_tag_t & cb_tag)
    {
        const auto & itr = _callbacks.find(ev);
        if(itr != _callbacks.end())
        {
            auto & cbs = itr->second;
            const auto & cb_itr = cbs.find(cb_tag);
            if(cb_itr != cbs.end())
                cbs.erase(cb_itr);
        }
    }

    template<typename ... args_t>
    void trigger(const event_t & ev, args_t ... args)
    {
        const auto & itr = _callbacks.find(ev);
        if(itr != _callbacks.end())
        {
            for(const auto & cb : itr->second)
            {
                cb->second(args ...);
            }
        }
    }

    
private:
    std::unordered_map<event_t, std::unordered_map<callback_tag_t, callback_t>> _callbacks;
};

FS_SUN_NS_END
