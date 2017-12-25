#pragma once
#include "ns.h"
#include "string.h"
#include "luastate.h"
#include <vector>

#define _GB_UTILS_LUATABLE_GETTER_DECL(ret_type, name)	\
    ret_type get_##name##_by_key(const char* key);	\
    ret_type get_##name##_by_idx(const size_t idx);	\
    std::vector<ret_type> get_##name##s();		

GB_UTILS_NS_BEGIN

class luatable
{
public:
    luatable(const char* file, luastate& config_luastate);
public:
    bool validate()const;
    
    _GB_UTILS_LUATABLE_GETTER_DECL(lua_Number, number);
    _GB_UTILS_LUATABLE_GETTER_DECL(lua_Integer, integer);
    _GB_UTILS_LUATABLE_GETTER_DECL(gb::utils::string, string);
    
    template<typename T> T get_table_by_key(const char* key)
	{
	    assert(key != nullptr);
	    lua_getfield(_l, -1, key);
	    if(lua_type(_l, -1) == LUA_TTABLE)
	    {
		T t(this);
		lua_pop(_l, 1);
		return t;
	    }
	    else
	    {
		lua_pop(_l, 1);
		return T();
	    }
	}
    
    template<typename T> T get_table_by_idx(const size_t idx)
	{
	    assert(idx >= 1);
	    lua_rawgeti(_l, -1, idx);
	    if(lua_type(_l, -1) == LUA_TTABLE)
	    {
		T t(this);
		lua_pop(_l, 1);
		return t;
	    }
	    else
	    {
		lua_pop(_l, 1);
		return T();
	    }
	}

    template<typename T> std::vector<T> get_tables()
	{
	    std::vector<T> ret;
	    const size_t len = lua_objlen(_l, -1);
	    for(size_t i = 1; i <= len; i++)
	    {
		ret.push_back(get_table_by_idx<T>(i));
	    }
	    return ret;
	}
    
    // scene::instantiate(const char* luafile)
    // 	{
    // 	    luatable lt(luafile);
    // 	    lt.get_cpps([](lua_State* l)
    // 			{
    // 			    Entity e;
    // 			    e.instantiate(l);//may also instantiate(luafile)//prefab instantiate
    // 			    _mpEntitys.insert(e.name, e);
    // 			});
    // 	}
    // void get_cpps(func)
    // 	{
    // 	    size_t len = lua_objlen(_l, -1);
    // 	    for(size_t i = 1; i <= len; i++)
    // 	    {
    // 		lua_rawgeti(_l, -1, i);
    // 		func(_l);

    // 		lua_pop(_l, 1);
    // 	    }
    // 	}
private:
    lua_State* _l;
};

GB_UTILS_NS_END
