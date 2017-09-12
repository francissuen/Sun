#include "gbLuaCPP.h"

//lua_State* gbLuaState;

lua_State* gbLuaCPP_init()
{
	lua_State* L = luaL_newstate();
	if (L == nullptr)
	{
		printf("nullptr");
		return nullptr;
	}
	//gbLuaState = L;
	luaL_openlibs(L);
	return L;
}

bool gbLuaCPP_dofile(lua_State* L, const char* szLuaFile)
{
	lua_pushcfunction(L, gbLuaTraceback);
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

bool gbLuaCPP_dostring(lua_State* L, const char* szLua)
{
	lua_pushcfunction(L, gbLuaTraceback);
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

int gbLuaTraceback(lua_State* L)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	lua_getfield(L, -1, "traceback");
	lua_pushvalue(L, 1);
	lua_call(L, 1, 1);

	printf("lua_traceback:%s\n", lua_tostring(L, -1));

	return 1;
}
