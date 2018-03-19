#pragma once
#include "ns.h"
extern "C"
{
#include <lua/lua.h>    
}
#include "common.h"

GB_UTILS_NS_BEGIN;

class luastate
{
public:
    luastate();
    ~luastate();
    bool dofile(const char* szLuaFile);
    bool dostring(const char* szLua);
    void append_packagepath(const char* path);
    lua_State* getstate();
private:
    lua_State* _l;

private:
    static int _debug_traceback(lua_State* l);
};

class luastate_mt
{
public:
    luastate_mt();
    ~luastate_mt();
    void dofile(const char* szLuaFile);
    void dostring(const char* szLua);
    luastate& operator[](const std::uint8_t threadIdx);
private:
    static std::uint8_t _threadCount;
    luastate* _ls;
};

// class luastate_mgr
// {
//     GB_SINGLETON_NO_CTORDEF(luastate_mgr);
//     ~luastate_mgr();
// public:
// 	/**
// 	*@param, if threadCount == 0, then using num of cores
// 	*/
//     void initialize(const unsigned char threadCount = 0);
//     luastate& getconfig_state(const unsigned char threadIdx = 0);
//     luastate& getlogic_state(const unsigned char threadIdx = 0);
// private:
//     unsigned char _threadCount;
//     luastate* _config_l;
//     luastate* _logic_l;
// };

GB_UTILS_NS_END;
