#include "luacpp.h"
#include <string>
//lua_State* fsLuaState;

lua_State* fsLuaCPP_init()
{
	lua_State* L = luaL_newstate();
	if (L == nullptr)
	{
		printf("nullptr");
		return nullptr;
	}
	//fsLuaState = L;
	luaL_openlibs(L);
	return L;
}

bool fsLuaCPP_dofile(lua_State* L, const char* szLuaFile)
{
	lua_pushcfunction(L, fsLuaTraceback);
	if (luaL_loadfile(L, szLuaFile) != 0)
	{
		printf("luaL_loadfile error:%s\n", lua_tostring(L, -1));
		return false;
	}
	else
	{
		return lua_pcall(L, 0, 0, -2) == 0 ? true : false;
	}
}

bool fsLuaCPP_dostring(lua_State* L, const char* szLua)
{
	lua_pushcfunction(L, fsLuaTraceback);
	if (luaL_loadstring(L, szLua) != 0)
	{
		printf("luaL_loadfile error:%s\n", lua_tostring(L, -1));
		return false;
	}
	else
	{
		return lua_pcall(L, 0, 0, -2) == 0 ? true : false;
	}
}

void fsLuaCPP_appendPackagePath(lua_State* L, const char* path)
{
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    std::string cur_path(lua_tostring(L, -1));
    cur_path += ";";
    cur_path += path;
    cur_path += "/?.lua";
    lua_pop(L, 1);
    lua_pushstring(L, cur_path.c_str());
    lua_setfield(L, -2, "path");
    lua_pop(L, 1);
}
int fsLuaTraceback(lua_State* L)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	lua_getfield(L, -1, "traceback");
	lua_pushvalue(L, 1);
	lua_call(L, 1, 1);

	printf("lua_traceback:%s\n", lua_tostring(L, -1));

	return 1;
}
