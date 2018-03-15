#pragma once

#include "ns.h"
#include <functional>
#include <unordered_map>
#include <string>

GB_UTILS_NS_BEGIN

#define GB_UTILS_CALLBACK_REG(cbObj, triggerKey, className, memFunc, ...) \
    cbObj.RegisterCB(triggerKey, #memFunc, std::bind(&(className::memFunc), this, __VA_ARGS__));

#define GB_UTILS_CALLBACK_UNREG(cbObj, triggerKey, memFunc)	\
    cbObj.UnregisterCB(triggerKey, #memFunc);
template <typename ... FuncParams>
class callback
{
public:
    void RegisterCB(const std::uint32_t triggerKey,
		    const char* szFuncName,
		    const std::function<void(FuncParams ...)>& cb)
	{
	    auto triggerItr = _mpCBs.find(triggerKey);
	    if (triggerItr != _mpCBs.end())
	    {
		triggerItr->second.insert(std::make_pair(std::string(szFuncName), cb));
	    }
	    else
		_mpCBs.insert(std::make_pair(triggerKey, std::unordered_map<std::string, std::function<void(void)>>{ {funcName, cb}}));

	}
    void UnregisterCB(const std::uint32_t triggerKey, const char* szFuncName)
	{
	    auto triggerItr = _mpCBs.find(triggerKey);
	    if (triggerItr != _mpCBs.end())
	    {
		triggerItr->second.erase(std::string(szFuncName));
	    }
	}
    void Trigger(const std::uint32_t triggerKey, FuncParams ... func_params)
	{
	    auto triggerItr = _mpCBs.find(triggerKey);
	    if (triggerItr != _mpCBs.end())
	    {
		const std::unordered_map<std::string, std::function<void(void)>>& cbs = triggerItr->second;

		std::for_each(cbs.begin(), cbs.end(), [](const std::pair<std::string, std::function<void(void)>>& cb)
			      {
				  cb.second(func_params ...);
			      });
	    }

	}
private:
    std::unordered_map<std::uint32_t, std::unordered_map<std::string, std::function<void(FuncParams ...)>>> _mpCBs;
};

GB_UTILS_NS_END

