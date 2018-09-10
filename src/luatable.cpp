#include "luatable.h"
#include "logger.h"

using namespace gb::utils;

luatable_mapper::luatable_mapper(luastate& ls):
    _l(ls.getstate()),
    _ls(ls),
    _UnbalancedStack(0)
{
    assert(_l != nullptr);
}

luatable_mapper::~luatable_mapper()
{
    unmap();
}

void luatable_mapper::_balanceStack()
{
    if(_UnbalancedStack == 0)
	return;
    
    lua_pop(_l, _UnbalancedStack);
    _UnbalancedStack = 0;
}

bool luatable_mapper::map_file(const char* file)
{
    _balanceStack();
    
    _Data = file;
    if(_ls.dofile(file) && _validate())
	return true;
    else
    {
	logger::Instance().error("luatable_mapper::map_file file@ " + _Data);
	return false;
    }
}

bool luatable_mapper::map_string(const char* luaCode)
{
    _balanceStack();
    
    _Data = luaCode;
    if(_ls.dostring(luaCode) && _validate())
	return true;
    else
    {
	logger::Instance().error("luatable_mapper::map_string fileCode@ " + _Data);
	return false;
    }
}

void luatable_mapper::unmap()
{
    _balanceStack();
    _ls.SetLock(false);
}

bool luatable_mapper::load_table(const char* key)
{
    lua_getfield(_l, -1, key);
    if(lua_type(_l, -1) == LUA_TTABLE)
    {
	_UnbalancedStack += 1;
	return true;
    }
    else
	return false;
}

void luatable_mapper::unload_table()
{
    lua_pop(_l, 1);
    _UnbalancedStack -= 1;
    assert(_UnbalancedStack >= 0);
}

bool luatable_mapper::_validate()
{
    if(lua_type(_l, -1) == LUA_TTABLE)
    {
	_UnbalancedStack += 1;
	_ls.SetLock(true);
	return true;
    }
    else
	return false;
}

size_t luatable_mapper::objlen()const
{
    return lua_objlen(_l, -1);
}

bool luatable_mapper::has_key(const char* key)const
{
    lua_getfield(_l, -1, key);
    bool ret = lua_type(_l, -1) != LUA_TNIL;
    lua_pop(_l, 1);
    return ret;
}

#define _GB_UTILS_LUATABLE_MAPPER_GETTER_DEF(ret_type, name, type, lua_to_func, default_value) \
    ret_type luatable_mapper::get_##name##_by_key(const char* key) const \
    {									\
	assert(key != nullptr);						\
	lua_getfield(_l, -1, key);					\
	if(lua_type(_l, -1) == type)					\
	{								\
	    ret_type ret(lua_to_func(_l, -1));				\
	    lua_pop(_l, 1);						\
	    return ret;							\
	}								\
	else								\
	{								\
	    logger::Instance().warning(string("luatable_mapper::get_" #name "_by_key unmatched type@") + key); \
	    lua_pop(_l, 1);						\
	    return default_value;					\
	}								\
    }									\
    ret_type luatable_mapper::get_##name() const			\
    {									\
	if(lua_type(_l, -1) == type)					\
	{								\
	    ret_type ret(lua_to_func(_l, -1));				\
	    lua_pop(_l, 1);						\
	    return ret;							\
	}								\
	else								\
	    return default_value;					\
    }									\
									\
    std::vector<ret_type> luatable_mapper::get_##name##s() const	\
    {									\
	std::vector<ret_type> ret;					\
	const size_t len = lua_objlen(_l, -1);				\
	for(size_t i = 1; i <= len; i++)				\
	{								\
	    ret.push_back(get_##name##_by_idx(i));			\
	}								\
	return ret;							\
    }									\
									\
    std::vector<ret_type> luatable_mapper::get_##name##s_by_key(const char* key) const \
    {									\
	GB_ASSERT(key != nullptr);					\
	std::vector<ret_type> ret;					\
	lua_getfield(_l, -1, key);					\
	const int lt = lua_type(_l, -1);				\
	if(lt == LUA_TTABLE)						\
	{								\
	    const size_t len = lua_objlen(_l, -1);			\
	    ret.reserve(len);						\
	    for(size_t i = 1; i <= len; i++)				\
	    {								\
		ret.push_back(get_##name##_by_idx(i));			\
	    }								\
	}								\
	else if(lt == type)						\
	    ret.push_back(get_##name());				\
	lua_pop(_l, 1);							\
	return ret;							\
    }									\
									\
    std::vector<ret_type> luatable_mapper::get_##name##s_by_idx(const size_t idx) const \
    {									\
	GB_ASSERT(idx >= 1);						\
	std::vector<ret_type> ret;					\
	lua_rawgeti(_l, -1, idx);					\
	const int tl = lua_type(_l, -1);				\
	if(tl == LUA_TTABLE)						\
	{								\
	    const size_t len = lua_objlen(_l, -1);			\
	    ret.reserve(len);						\
	    for(size_t i = 1; i <= len; i++)				\
	    {								\
		ret.push_back(get_##name##_by_idx(i));			\
	    }								\
	}								\
	else if(tl == type)						\
	    ret.push_back(get_##name());				\
	lua_pop(_l, 1);							\
	return ret;							\
    }									\
									\
    ret_type luatable_mapper::get_##name##_by_idx(const size_t idx) const \
    {									\
	assert(idx >= 1);						\
	lua_rawgeti(_l, -1, idx);					\
	if(lua_type(_l, -1) == type)					\
	{								\
	    ret_type ret(lua_to_func(_l, -1));				\
	    lua_pop(_l, 1);						\
	    return ret;							\
	}								\
	else								\
	{								\
	    logger::Instance().warning(string("luatable_mapper::get_" #name "_by_idx unmatched type@") + idx); \
	    lua_pop(_l, 1);						\
	    return default_value;					\
	}								\
    }									


_GB_UTILS_LUATABLE_MAPPER_GETTER_DEF(lua_Number, number, LUA_TNUMBER, lua_tonumber, 0);
_GB_UTILS_LUATABLE_MAPPER_GETTER_DEF(lua_Integer, integer, LUA_TNUMBER, lua_tointeger, 0);
_GB_UTILS_LUATABLE_MAPPER_GETTER_DEF(string, string, LUA_TSTRING, lua_tostring, string());

static __forceinline bool _lua_toboolean_to_bool(lua_State *L, int idx)
{
	return lua_toboolean(L, idx) ? true : false;
}
_GB_UTILS_LUATABLE_MAPPER_GETTER_DEF(bool, boolean, LUA_TBOOLEAN, _lua_toboolean_to_bool, false);

void luatable_mapper::for_each(const std::function<void(const size_t idx)>& func) const
{
    const size_t len = lua_objlen(_l, -1);
    for(size_t i = 1; i <= len; i++)
    {
	func(i);
    }
}
void luatable_mapper::for_each_in(const std::function<void(const size_t)>& func, const char* key) const
{
    GB_ASSERT( key != nullptr);
    lua_getfield(_l, -1, key);
    if(lua_type(_l, -1) == LUA_TTABLE)
	for_each(func);
    else
	logger::Instance().warning(string("luatable_mapper::for_each_in, type not a table key@ ") + key);
    lua_pop(_l, 1);
}
void luatable_mapper::for_each_in(const std::function<void(const size_t)>& func, const size_t idx) const
{
    GB_ASSERT( idx >= 1 );
    lua_rawgeti(_l, -1, idx);
    if(lua_type(_l, -1) == LUA_TTABLE)
	for_each(func);
    else
	logger::Instance().warning(string("luatable_mapper::for_each_in, type not a table idx@ ") + idx);
    lua_pop(_l, 1);
}
