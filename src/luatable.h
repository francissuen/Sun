#pragma once
#include "ns.h"
extern "C"
{
#include <lua/include/lua.h>
}
#include "string.h"
#define _GB_UTILS_LUATABLE_GETTER_DECL(ret_type, name)	\
    ret_type get_##name##_by_key(const char* key);	\
    ret_type get_##name##_by_idx(const const int idx); 

GB_UTILS_NS_BEGIN

class luatable
{
    luatable(const char* file, lua_State* config_luaState)
	{
	    ...
	    if no a table
		      throw err
	}
public:
    _GB_UTILS_LUATABLE_GETTER_DECL(lua_Number, number);
    _GB_UTILS_LUATABLE_GETTER_DECL(lua_Integer, integer);
    _GB_UTILS_LUATABLE_GETTER_DECL(gb::utils::string, string);

    scene::instantiate(const char* luafile)
	{
	    luatable lt(luafile);
	    lt.get_cpps([](lua_State* l)
			{
			    Entity e;
			    e.instantiate(l);//may also instantiate(luafile)//prefab instantiate
			    _mpEntitys.insert(e.name, e);
			});
	}
    void get_cpps(func)
	{
	    size_t len = lua_objlen(_l, -1);
	    for(size_t i = 1; i <= len; i++)
	    {
		lua_rawgeti(_l, -1, i);
		func(_l);

		lua_pop(_l, 1);
	    }
	}
private:
    lua_State _l;
};

GB_UTILS_NS_END
