#include "luatable.h"
#include "logger.h"

using namespace gb::utils;

#define _GB_UTILS_LUATABLE_GETTER_DEF(ret_type, name, type, lua_to_func, default_value) \
    ret_type luatable::get_##name##_by_key(const char* key)		\
    {									\
	assert(key != nullptr);						\
	lua_getfield(_l, -1, key);					\
	if(lua_type(_1, -1) == type)					\
	    return lua_to_func(_l, -1)					\
	    else							\
	    {								\
		logger::Instance().warning(string("luatable::get_" #name "_by_key unmatched type@") + key); \
		return default_value;					\
	    }								\
    }									\
    ret_type luatable::get_##name##_by_idx(const int idx)		\
    {									\
	assert(idx >= 1);						\
	lua_rawgeti(_l, -1, idx);					\
	if(lua_type(_1, -1) == type)					\
	    return lua_to_func(_l, -1)					\
	    else							\
	    {								\
		logger::Instance().warning(string("luatable::get_" #name "_by_idx unmatched type@") + idx); \
		return default_value;					\
	    }								\
    }

_GB_UTILS_LUATABLE_GETTER_DEF(lua_Number, number, LUA_TNUMBER, lua_tonumber, 0);
_GB_UTILS_LUATABLE_GETTER_DEF(lua_Integer, integer, LUA_TNUMBER, lua_tointeger, 0);
_GB_UTILS_LUATABLE_GETTER_DEF(string, string, LUA_TSTRING, lua_tostring, string());
