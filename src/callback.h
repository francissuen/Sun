#pragma once

#include "ns.h"
#include <functional>
#include <unordered_map>
#include <algorithm>

#include "string.h"
#include "logger.h"
GB_UTILS_NS_BEGIN

#define GB_UTILS_CALLBACK_REG(cbObj, memFunc, ...)			\
    cbObj.RegisterCB((std::uintptr_t)this, std::bind(&memFunc, this, __VA_ARGS__));

#define GB_UTILS_CALLBACK_REG_1P(cbObj, memFunc)	\
    GB_UTILS_CALLBACK_REG(cbObj, memFunc, std::placeholders::_1)

#define GB_UTILS_CALLBACK_REG_2P(cbObj, memFunc)	\
    GB_UTILS_CALLBACK_REG(cbObj, memFunc, std::placeholders::_1, std::placeholders::_2)

#define GB_UTILS_CALLBACK_UNREG(cbObj, memFunc)		\
    cbObj.UnregisterCB((std::uintptr_t)this);

template <typename ... FuncParams>
class callback
{
public:
    callback(){}
    callback(const std::uintptr_t instanceAddr,
	     const std::function<void(FuncParams ...)>& cb):
	_mpCBs{{{instanceAddr, cb}}}
	{
	}
public:
    void RegisterCB(const std::uintptr_t instanceAddr,
		    const std::function<void(FuncParams ...)>& cb)
	{
	    const auto ret = _mpCBs.insert(std::make_pair(instanceAddr, cb));
	    if(!(ret.second))
		logger::Instance().warning("callback::RegisterCB re-regiter error");
	}
    
    void UnregisterCB(const std::uintptr_t instanceAddr)
	{
	    _mpCBs.erase(instanceAddr);
	}

    void Trigger(FuncParams ... func_params)
	{
	    std::for_each(_mpCBs.begin(), _mpCBs.end(), [&func_params ...](const std::pair<std::uintptr_t, std::function<void(FuncParams ...)>>& f)
			  {
			      f.second(func_params ...);
			  });
	}
private:
    std::unordered_map<std::uintptr_t, std::function<void(FuncParams ...)>> _mpCBs;
};

#define GB_UTILS_MULTI_CALLBACK_REG(cbObj, triggerKey, memFunc, ...)	\
    cbObj.RegisterCB(triggerKey, (std::uintptr_t)this, std::bind(&memFunc, this, __VA_ARGS__));

#define GB_UTILS_MULTI_CALLBACK_REG_1P(cbObj, triggerKey, memFunc)	\
    GB_UTILS_MULTI_CALLBACK_REG(cbObj, triggerKey, memFunc, std::placeholders::_1);

#define GB_UTILS_MULTI_CALLBACK_REG_2P(cbObj, triggerKey, memFunc)	\
    GB_UTILS_MULTI_CALLBACK_REG(cbObj, triggerKey, memFunc, std::placeholders::_1, std::Placeholders::_2);

#define GB_UTILS_MULTI_CALLBACK_UNREG(cbObj, triggerKey, memFunc)	\
    cbObj.UnregisterCB(triggerKey, (std::uintptr_t)this);

template <typename ... FuncParams>
class multi_callback
{
public:
    void RegisterCB(const std::uint32_t triggerKey,
		    const std::uintptr_t instanceAddr,
		    const std::function<void(FuncParams ...)>& cb)
	{
	    auto triggerItr = _mpCBs.find(triggerKey);
	    if (triggerItr != _mpCBs.end())
	    {
		triggerItr->second.RegisterCB(instanceAddr, cb);
	    }
	    else
		_mpCBs.insert(std::make_pair(triggerKey, callback<FuncParams ...>(instanceAddr, cb)));
	}
    
    void UnregisterCB(const std::uint32_t triggerKey, const std::uintptr_t instanceAddr)
	{
	    auto triggerItr = _mpCBs.find(triggerKey);
	    if (triggerItr != _mpCBs.end())
		triggerItr->second.UnregisterCB(instanceAddr);
	}
    
    void Trigger(const std::uint32_t triggerKey, FuncParams ... func_params)
	{
	    auto triggerItr = _mpCBs.find(triggerKey);
	    if (triggerItr != _mpCBs.end())
		triggerItr->second.Trigger(func_params ...);
	}
private:
    std::unordered_map<std::uint32_t, callback<FuncParams ...>> _mpCBs;
};

GB_UTILS_NS_END

