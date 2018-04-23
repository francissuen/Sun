#pragma once

#include "ns.h"
#include <functional>
#include <unordered_map>
#include <algorithm>

#include "string.h"
#include "logger.h"
GB_UTILS_NS_BEGIN

#define GB_UTILS_CALLBACK_REG(cbObj, memFunc, ...)			\
    cbObj.RegisterCB(this, std::bind(&memFunc, this, __VA_ARGS__));

#define GB_UTILS_CALLBACK_REG_1P(cbObj, memFunc)	\
    GB_UTILS_CALLBACK_REG(cbObj, memFunc, std::placeholders::_1)

#define GB_UTILS_CALLBACK_REG_2P(cbObj, memFunc)	\
    GB_UTILS_CALLBACK_REG(cbObj, memFunc, std::placeholders::_1, std::placeholders::_2)

#define GB_UTILS_CALLBACK_UNREG(cbObj, memFunc)		\
    cbObj.UnregisterCB(this);

template <typename ... FuncParams>
class callback
{
public:
    callback(){}
    callback(const void* inst,
	     const std::function<void(FuncParams ...)>& cb):
	_mpCBs{{{inst, cb}}}
	{
	}
public:
    void RegisterCB(const void* inst,
		    const std::function<void(FuncParams ...)>& cb)
	{
	    const auto ret = _mpCBs.insert(std::make_pair(inst, cb));
	    if(!(ret.second))
		logger::Instance().warning("callback::RegisterCB re-regiter error");
	}
    
    void UnregisterCB(const void* inst)
	{
	    _mpCBs.erase(inst);
	}

    void Trigger(FuncParams ... func_params)
	{
	    std::for_each(_mpCBs.begin(), _mpCBs.end(), [&func_params ...](const std::pair<const void*, std::function<void(FuncParams ...)>>& f)
			  {
			      f.second(func_params ...);
			  });
	}
    void Trigger(const void* inst, FuncParams ... func_params)
    {
	const auto iter = _mpCBs.find(inst);
	if(iter != _mpCBs.end())
	    iter->second(func_params ...);
    }
private:
    std::unordered_map<const void*, std::function<void(FuncParams ...)>> _mpCBs;
};

#define GB_UTILS_MULTI_CALLBACK_REG(cbObj, triggerKey, memFunc, ...)	\
    cbObj.RegisterCB(triggerKey, this, std::bind(&memFunc, this, __VA_ARGS__));

#define GB_UTILS_MULTI_CALLBACK_REG_1P(cbObj, triggerKey, memFunc)	\
    GB_UTILS_MULTI_CALLBACK_REG(cbObj, triggerKey, memFunc, std::placeholders::_1);

#define GB_UTILS_MULTI_CALLBACK_REG_2P(cbObj, triggerKey, memFunc)	\
    GB_UTILS_MULTI_CALLBACK_REG(cbObj, triggerKey, memFunc, std::placeholders::_1, std::Placeholders::_2);

#define GB_UTILS_MULTI_CALLBACK_UNREG(cbObj, triggerKey, memFunc)	\
    cbObj.UnregisterCB(triggerKey, this);

template <typename Key_t, typename ... FuncParams>
class multi_callback
{
public:
    void RegisterCB(const Key_t triggerKey,
		    const void* inst,
		    const std::function<void(FuncParams ...)>& cb)
	{
	    auto triggerItr = _mpCBs.find(triggerKey);
	    if (triggerItr != _mpCBs.end())
	    {
		triggerItr->second.RegisterCB(inst, cb);
	    }
	    else
		_mpCBs.insert(std::make_pair(triggerKey, callback<FuncParams ...>(inst, cb)));
	}
    
    void UnregisterCB(const Key_t triggerKey, const void* inst)
	{
	    auto triggerItr = _mpCBs.find(triggerKey);
	    if (triggerItr != _mpCBs.end())
		triggerItr->second.UnregisterCB(inst);
	}
    
    void Trigger(const Key_t triggerKey, FuncParams ... func_params)
	{
	    auto triggerItr = _mpCBs.find(triggerKey);
	    if (triggerItr != _mpCBs.end())
		triggerItr->second.Trigger(func_params ...);
	}
private:
    std::unordered_map<Key_t, callback<FuncParams ...>> _mpCBs;
};

GB_UTILS_NS_END

