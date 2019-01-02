#pragma once
#include "fsCommonConfig.h"
#include <cstdint>
#define fsLUAAPI 1


//lua 5.1 reference manual:http://www.lua.org/manual/5.1/manual.html

#ifdef fsLUAAPI

extern "C"
{
#include <lua/include/lua.h>
#include <lua/include/lualib.h>
#include <lua/include/lauxlib.h>
}

//todo, re-link lua lib in other project issue

#include <vector>
#include <cstring>

#define fs_LC_MAX_METATABLENAME_LEN 64

#define fsLC_METATABLE_SUFFIX "_mt"

#define MTNAME_(type) #type fsLC_METATABLE_SUFFIX
#define MTNAME(type) MTNAME_(type)

#define fsUSERDATA "fs_userdata"
#endif

#ifdef fsLUAAPI
#define fs_LC_Register(ClassName)			\
    private:						\
    typedef fsLuaCPP<ClassName>::fs_LC_Func fs_LC_Func;	\
public:							\
static void fs_LC_Reg(lua_State* L/*, const char* szParentTable = nullptr*/);				
//inline int fs_LC_EF_Destructor(lua_State* L){delete this;}
#else
#define fs_LC_Register(ClassName)
#endif

// #ifdef fsLUAAPI
// #define fs_LC_Register_PrvCns(ClassName) \
//   private: \
//   typedef fsLuaCPP_PrvCns<ClassName>::fs_LC_Func fs_LC_Func; \
// public: \
//  static void fs_LC_Reg(lua_State* L);	
// #else
// #define fs_LC_Register_PrvCns(ClassName)
// #endif

// #ifdef fsLUAAPI
// //used after register
// #define fs_LC_Singleton_Instance_Reg(ClassName)				\
//     lua_newtable(L);							\
//     void* p = lua_newuserdata(L, sizeof(void*));			\
//     *(std::uintptr_t*)p = (std::uintptr_t)&(ClassName::Instance);	\
//     luaL_getmetatable(L, MTNAME(ClassName));				\
//     lua_setmetatable(L, -2);						\
//     lua_setfield(L, -2, "Instance");					\
//     lua_setglobal(L, ClassName);	
// #else
// #define fs_LC_Singleton_Instance_Reg(ClassName)
// #endif

#ifdef fsLUAAPI
//actual pointer wrapped by a data which point to the pointer
inline int fs_LC_push_userdata(lua_State* L, std::uintptr_t ud, const char* mtName)
{
    if (luaL_newmetatable(L, "fs_userdata") != 0) 
    { 
	/*weaktable ?*/ 
    } 
    lua_rawgeti(L, -1, ud);
    if (lua_type(L, -1) != LUA_TUSERDATA)
    {
	//a wrapper
	void* p = lua_newuserdata(L, sizeof(void*));
	memcpy(p, &ud, sizeof(void*));
	luaL_getmetatable(L, mtName);
	if (lua_istable(L, -1))
	    lua_setmetatable(L, -2);
	else
	    lua_pop(L, 1);

	//push to return, in case lua_rawseti pop the value 
	lua_pushvalue(L, -1);

	lua_rawseti(L, -4, ud);
    }
    return 1;
}
#endif

// #ifdef fsLUAAPI
// #define fs_LC_EXPORT_INLINE_FUNC_1R_LUD_0P(ret, name, definition)	\
//     public:								\
//     inline ret* name() definition					\
//     private:								\
//     inline int fs_LC_EF_##name(lua_State* L)				\
//     {									\
//         unsigned int ud = (unsigned int)(name());			\
//         return fs_LC_push_userdata(L, ud, MTNAME(ret));			\
//     }
// #else
// #define fs_LC_EXPORT_INLINE_FUNC_1R_LUD_0P(ret, name, definition)	\
//     public:								\
//     inline ret* name() definition 
// #endif

// #ifdef fsLUAAPI
// #define fs_LC_EXPORT_INLINE_FUNC_1R_0P(ret, ret_pusher, name, definition) \
//     public:								\
//     inline ret name() definition					\
//     private:								\
//     inline int fs_LC_EF_##name(lua_State* L)				\
//     {									\
// 	ret_pusher(L, name());						\
// 	return 1;							\
//     }
// #else
// #define fs_LC_EXPORT_INLINE_FUNC_1R_0P(ret, ret_pusher, name, definition) \
//     public:								\
//     inline ret name() definition 
// #endif

// #ifdef fsLUAAPI
// #define fs_LC_EXPORT_FUNC_1R_0P(ret, ret_pusher, name)	\
//     public:						\
//     ret name();						\
// private:						\
// inline int fs_LC_EF_##name(lua_State* L)		\
// {							\
//     ret_pusher(L, name());				\
//     return 1;						\
// }
// #else
// #define fs_LC_EXPORT_FUNC_1R_0P(ret, ret_pusher, name)	\
//     public:						\
//     ret name(); 
// #endif

// #ifdef fsLUAAPI
// #define fs_LC_EXPORT_INLINE_FUNC_0R_1P_TABLE(name, tp, p_name, p_getter, definition) \
//     public:								\
//     inline void name(tp& p_name) definition				\
//     private:								\
//     inline int fs_LC_EF_##name(lua_State* L)				\
//     {									\
// 	tp arg;								\
// 	p_getter(L, 2, arg);						\
// 	name(arg);							\
// 	return 0;							\
//     }
// #else
// #define fs_LC_EXPORT_INLINE_FUNC_0R_1P_TABLE(name, tp, p_name, p_getter, definition) \
//     public:								\
//     inline void name(tp p_name) definition 
// #endif

// #ifdef fsLUAAPI
// #define fs_LC_EXPORT_FUNC_0R_2P(name, tp1, p1_getter, tp2, p2_getter)	\
//     public:								\
//     void name(tp1, tp2);						\
// private:								\
// inline int fs_LC_EF_##name(lua_State* L)				\
// {									\
//     tp1 p1 = p1_getter(L, 2);						\
//     tp2 p2 = p2_getter(L, 3);						\
//     name(p1, p2);							\
//     return 0;								\
// }
// #else
// #define fs_LC_EXPORT_FUNC_0R_2P(name, tp1, p1_getter, tp2, p2_getter)	\
//     public:								\
//     void name(tp1, tp2); 
// #endif

// #ifdef fsLUAAPI
// #define fs_LC_EXPORT_FUNC_0R_0P(name)		\
//     public:					\
//     void name();				\
// private:					\
// inline int fs_LC_EF_##name(lua_State* L)	\
// {						\
//     name();					\
//     return 0;					\
// }
// #else
// #define fs_LC_EXPORT_FUNC_0R_0P(name)		\
//     public:					\
//     void name(); 
// #endif

// #ifdef fsLUAAPI
// #define fs_LC_EXPORT_INLINE_FUNC_0R_0P(name, definition)	\
//     public:							\
//     inline void name() definition				\
//     private:							\
//     inline int fs_LC_EF_##name(lua_State* L)			\
//     {								\
// 	name();							\
// 	return 0;						\
//     }
// #else
// #define fs_LC_EXPORT_INLINE_FUNC_0R_0P(name, definition)	\
//     public:							\
//     inline void name() definition 
// #endif

// #ifdef fsLUAAPI
// #define fs_LC_EXPORT_INLINE_FUNC_0R_1P(name, tp, p, definition, p_getter) \
//     public:								\
//     void name(tp p) definition						\
//     private:								\
//     inline int fs_LC_EF_##name(lua_State* L)				\
//     {									\
// 	tp arg = (tp)p_getter(L, 2);					\
// 	name(arg);							\
// 	return 0;							\
//     }
// #else
// #define fs_LC_EXPORT_INLINE_FUNC_0R_1P(name, tp, p, definition, p_getter) \
//     public:								\
//     void name(); 
// #endif

// #ifdef fsLUAAPI
// #define fs_LC_EXPORT_FUNC_0R_1P(name, tp, p_getter)	\
//     public:						\
//     void name(tp);					\
// private:						\
// inline int fs_LC_EF_##name(lua_State* L)		\
// {							\
//     tp arg = p_getter(L, 2);				\
//     name(arg);						\
//     return 0;						\
// }
// #else
// #define fs_LC_EXPORT_FUNC_0R_0P(name)		\
//     public:					\
//     void name(); 
// #endif

// #ifdef fsLUAAPI
// #define fs_LC_EXPORT_FUNC_1R_1P(ret, ret_pusher, name, arg_type, arg_geter) \
//     public:								\
//     ret name(arg_type);							\
// private:								\
// inline int fs_LC_EF_##name(lua_State* L)				\
// {									\
//     arg_type arg = arg_geter(L, 2);					\
//     ret_pusher(L, name(arg));\						\
// 				 return 1;				\
// }
// #else
// #define fs_LC_EXPORT_FUNC_1R_1P(ret, ret_pusher, name, arg_type, arg_geter) \
//     public:								\
//     ret name(arg_type);
// #endif

// #ifdef fsLUAAPI
// #define fs_LC_EXPORT_FUNC_1R_LUD_1P(ret, name, arg_type, arg_geter)	\
//     public:								\
//     ret* name(arg_type);						\
// private:								\
// inline int fs_LC_EF_##name(lua_State* L)				\
// {									\
//     arg_type arg = arg_geter(L, 2);					\
//     unsigned int ud = (unsigned int)(name(arg));			\
//     return fs_LC_push_userdata(L, ud, MTNAME(ret));			\
// }
// #else
// #define fs_LC_EXPORT_FUNC_1R_LUD_1P(ret, name, arg_type, arg_geter)	\
//     public:								\
//     ret* name(arg_type); 
// #endif

// #ifdef fsLUAAPI
// #define fs_LC_EXPORT_INLINE_FUNC_1R_LUD_1P(ret, name, definition, param, arg_type, arg_geter) \
//     public:								\
//     inline ret* name(arg_type param) definition				\
//     private:								\
//     inline int fs_LC_EF_##name(lua_State* L)				\
//     {									\
// 	arg_type arg = arg_geter(L, 2);					\
// 	unsigned int ud = (unsigned int)(name(arg));			\
// 	return fs_LC_push_userdata(L, ud, MTNAME(ret));			\
//     }
// #else
// #define fs_LC_EXPORT_INLINE_FUNC_1R_LUD_1P(ret, name, definition, arg_type, arg_geter) \
//     public:								\
//     inline ret* name(arg_type param) definition 
// #endif

// #ifdef fsLUAAPI
// #define fs_LC_MKFUNCNAME(name) fs_LC_EF_##name
// #else
// #define fs_LC_MKFUNCNAME(name) name
// #endif


#ifdef fsLUAAPI
#define fs_LC_EXPORT_FUNC(name)			\
    private:					\
    inline int fs_LC_EF_##name(lua_State* L)
#else
#define fs_LC_EXPORT_FUNC(name)
#endif

// #ifdef fsLUAAPI
// #define fs_LC_EXPORT_WRAP_FUNC(name, definition)	\
//     private:						\
//     inline int fs_LC_EF_##name(lua_State* L) definition
// #else
// #define fs_LC_EXPORT_WRAP_FUNC(name, definition) 
// #endif

#ifdef fsLUAAPI
//read only
#define fs_LC_EXPORT_VAR_R(type, name, push_method)	\
    public:						\
    type name;						\
private:						\
inline int fs_LC_EV_R_##name(lua_State* L)		\
{							\
    push_method(L, name);				\
    return 1;						\
}
#else
#define fs_LC_EXPORT_VAR_R(type, name, push_method)	\
    public:						\
    type name;
#endif

#ifdef fsLUAAPI
//read&write
#define fs_LC_EXPORT_VAR_RW(type, name, push_method, pop_method)	\
    public:								\
    type name;								\
private:								\
inline int fs_LC_EV_R_##name(lua_State* L)				\
{									\
    push_method(L, name);						\
    return 1;								\
}									\
inline int fs_LC_EV_W_##name(lua_State* L)				\
{									\
    name = pop_method(L, 1);						\
    return 1;								\
}
#else
#define fs_LC_EXPORT_VAR_RW(type, name, push_method, pop_method)	\
    public:								\
    type name;	
#endif

#ifdef fsLUAAPI

//#ifdef fsCommonSTATIC
//extern lua_State* fsLuaState;
//#else
//#ifdef fsCommonEXPORT
//extern _declspec(dllexport)lua_State* fsLuaState;
//#else
//extern _declspec(dllimport)lua_State* fsLuaState;
//#endif
//#endif

#ifdef fsCommonSTATIC
int fsLuaTraceback(lua_State* L);
#else
#ifdef fsCommonEXPORT
_declspec(dllexport) int fsLuaTraceback(lua_State* L);
#else
_declspec(dllimport) int fsLuaTraceback(lua_State* L);
#endif
#endif

// #ifdef fsCommonSTATIC
// lua_State* fsLuaCPP_init();
// #else
// #ifdef fsCommonEXPORT
// _declspec(dllexport) lua_State* fsLuaCPP_init();
// #else
// _declspec(dllimport) lua_State* fsLuaCPP_init();
// #endif
// #endif

#ifdef fsCommonSTATIC
bool fsLuaCPP_dofile(lua_State* L, const char* szLuaFile);
#else
#ifdef fsCommonEXPORT
_declspec(dllexport) bool fsLuaCPP_dofile(lua_State* L, const char* szLuaFile);
#else
_declspec(dllimport) bool fsLuaCPP_dofile(lua_State* L, const char* szLuaFile);
#endif
#endif

#ifdef fsCommonSTATIC
bool fsLuaCPP_dostring(lua_State* L, const char* szLua);
#else
#ifdef fsCommonEXPORT
_declspec(dllexport) bool fsLuaCPP_dostring(lua_State* L, const char* szLua);
#else
_declspec(dllimport) bool fsLuaCPP_dostring(lua_State* L, const char* szLua);
#endif
#endif

#ifdef fsCommonSTATIC
void fsLuaCPP_appendPackagePath(lua_State* L, const char* path);
#else
#ifdef fsCommonEXPORT
_declspec(dllexport) void fsLuaCPP_appendPackagePath(lua_State* L, const char* path);
#else
_declspec(dllimport) void fsLuaCPP_appendPackagePath(lua_State* L, const char* path);
#endif
#endif


//#ifdef fsRCDLLEXPORT
//_declspec(dllexport) int fsLuaTraceback(lua_State* L);
//#else
//_declspec(dllimport) int fsLuaTraceback(lua_State* L);
//#endif

// #define fs_LC(className)			\
//     className::fs_LC_Reg(L);

#define fs_LC_Func_Def				\
    std::vector<fs_LC_Func> funcs;

#define fs_LC_Func_push(name, pointer)		\
    funcs.push_back(fs_LC_Func(name, pointer))

#define fs_LC_CAST_LUD_TO(type, ud) (type)(*(int*)ud)

struct fsLCLuaFunc
{
    inline fsLCLuaFunc() :tblIdx(0), funcIdx(0)/*, argc(1), retc(0)*/ {}
    int tblIdx;
    int funcIdx;
    //unsigned int argc;
    //unsigned int retc;
};

// class fsLCString
// {
// public:
//     inline  fsLCString() :_data(nullptr) {}
//     inline  fsLCString(const char* val) : _size(strlen(val) + 1), _data(new char[_size])
// 	{
// 	    memcpy(_data, val, _size - 1);
// 	    _data[_size - 1] = '\0';
// 	}
//     inline ~fsLCString()
// 	{
// 	    if (_data != nullptr)
// 	    {
// 		delete[] _data;
// 		_data = nullptr;
// 	    }
// 	}
// public:
//     inline operator const char*()
// 	{
// 	    return _data;
// 	}
//     inline fsLCString& operator=(const char* val)
// 	{
// 	    if (_data != nullptr)
// 	    {
// 		delete[] _data;
// 		_data = nullptr;
// 	    }
// 	    _size = strlen(val) + 1;
// 	    _data = new char[_size];
// 	    memcpy(_data, val, _size - 1);
// 	    _data[_size - 1] = '\0';
// 	    return *this;
// 	}
//     inline  fsLCString& operator=(const fsLCString& other)
// 	{
// 	    if (_data != nullptr)
// 	    {
// 		delete[] _data;
// 		_data = nullptr;
// 	    }
// 	    _size = other.GetSize();
// 	    _data = new char[_size];
// 	    memcpy(_data, other.GetData(), _size - 1);
// 	    _data[_size - 1] = '\0';
// 	    return *this;
// 	}
// public:
//     inline unsigned int GetSize()const { return _size; }
//     inline const char* GetData()const { return _data; }
// private:
//     unsigned int _size;
//     char* _data;
// };

//inline int fs_LC_CallLuaFunc(lua_State* L, const fsLCLuaFunc& func)
//{
//	lua_rawgeti(L, LUA_REGISTRYINDEX, func.funcIdx);
//	lua_rawgeti(L, LUA_REGISTRYINDEX, func.tblIdx);
//	lua_call(L, func.argc, func.retc);
//	return func.retc;
//}

inline int fs_LC_RegisterLuaFunc(lua_State* L, int& tblIdx, int& funcIdx)
{
    //target
    if (!lua_istable(L, 2))
    {
	printf("RegisterLuaFunc not a table");
    }
    else
    {
	//make sure val at top
	lua_pushvalue(L, 2);
	tblIdx = luaL_ref(L, LUA_REGISTRYINDEX);
	if (tblIdx == LUA_REFNIL)
	{
	    printf("RegisterLuaFunc table == LUA_REFNIL");
	}
	else
	{
	    if (!lua_isfunction(L, 3))
	    {
		printf("RegisterLuaFunc not a function");
	    }
	    else
	    {
		lua_pushvalue(L, 3);
		funcIdx = luaL_ref(L, LUA_REGISTRYINDEX);
		if (funcIdx == LUA_REFNIL)
		{
		    printf("RegisterLuaFunc func == LUA_REFNIL");
		}
		else
		{
		    lua_pushinteger(L, tblIdx);
		    return 1;
		}
	    }
	}
    }
    //lua_pushnil(L);
    //lua_pushnil(L);
    return 0;
}

typedef int(*cpp_static_func)(lua_State* L);
struct fs_LC_StaticFunc
{
    inline fs_LC_StaticFunc(const char* name, cpp_static_func func) :Name(name), Func(func) {}
    const char* Name;
    cpp_static_func Func;

};

inline bool fs_LC_RegisterStaticFunc(lua_State* L, const std::vector<fs_LC_StaticFunc>& funcs, const char* ClassMetatableName)
{
    lua_getfield(L, LUA_REGISTRYINDEX, ClassMetatableName);
    if (!lua_istable(L, -1))
    {
	printf("fs_LC_RegisterStateFunc !lua_istable(L, -1)");
	return false;
    }

    for (std::vector<fs_LC_StaticFunc>::const_iterator i = funcs.cbegin(); i != funcs.cend(); i++)
    {
	lua_pushcclosure(L, (*i).Func, 0);
	lua_setfield(L, -2, (*i).Name);
    }
    return true;
}

template<typename T>
class fsLuaCPP
{
    typedef int (T::*cpp_func)(lua_State* L);

    static std::vector<cpp_func> Funcs;

public:
    struct fs_LC_Func
    {
	inline fs_LC_Func(const char* name, cpp_func func) :Name(name), Func(func) {}
	const char* Name;
	cpp_func Func;

    };


public:
    static void Register(lua_State* L, const char* name, const std::vector<fs_LC_Func>& funcs)
	{
	    //create metatable
	    sprintf(ClassMetatableName, "%s_mt", name);

	    if (luaL_newmetatable(L, ClassMetatableName) != 1)
	    {
		printf("luaL_newmetatable_err:%s", ClassMetatableName);
		return;
	    }
	    lua_pushstring(L, "__index");
	    lua_pushvalue(L, -2);
	    lua_settable(L, -3);

	    //member function
	    for (int i = 0; i < funcs.size(); i++)
	    {
		Funcs.push_back(funcs[i].Func);
		//push the index in to stack
		//int index = Funcs.size() - 1;
		lua_pushnumber(L, i);

		//associate index with the c func
		lua_pushcclosure(L, Func, 1);
		lua_setfield(L, -2, funcs[i].Name);
	    }

	    //member variable

	    //constructor    
	    // lua_pushcfunction(L, &fsLuaCPP::Constructor);
	    // lua_setglobal(L, name);
	}

    static void RegisterSingleton(lua_State* L, const char* ClassName)
	{
	    lua_newtable(L);							
	    void* p = lua_newuserdata(L, sizeof(void*));			
	    *(std::uintptr_t*)p = (std::uintptr_t)&(T::Instance());
	    luaL_getmetatable(L, ClassMetatableName);			
	    lua_setmetatable(L, -2);					
	    lua_setfield(L, -2, "Instance");
	    // if(szParentTable != nullptr)
	    // {
	    // 	lua_getfield(L, LUA_GLOBALSINDEX, szParentTable);
	    // 	if(lua_type(L, -1) == LUA_TTABLE)
	    // 	{
	    // 	    lua_pushvalue(L, -2);
	    // 	    lua_setfield(L, -2, ClassName);
	    // 	    return;
	    // 	}
	    // 	else
	    // 	    ;//not a table
	    // }
	    lua_setglobal(L, ClassName);	
	}

    static inline void RegisterBase(lua_State*L, const char* baseNameMT)
	{
	    lua_getfield(L, LUA_REGISTRYINDEX, ClassMetatableName);
	    if (!lua_istable(L, -1))
	    {
		printf("fsLuaCPP_PrvCns derived mt not found.");
		return;
	    }
	    lua_getfield(L, LUA_REGISTRYINDEX, baseNameMT);
	    if (!lua_istable(L, -1))
	    {
		printf("fsLuaCPP_PrvCns base mt not found.");
		return;
	    }
	    lua_setmetatable(L, -2);
	}

    static int Func(lua_State* L)
	{
	    int index = lua_tonumber(L, lua_upvalueindex(1));
	    T** ud = reinterpret_cast<T**>(lua_touserdata(L, 1));

	    //return ((*ud)->*(Funcs[index]))(L);

	    if (ud == nullptr)
	    {
		printf("Func ud == nullptr");
		return 0;
	    }
	    else
		//first parameter will always be the instance, and it's a pointer to pointer
		return ((*ud)->*(Funcs[index]))(L);
	}

    static char ClassMetatableName[fs_LC_MAX_METATABLENAME_LEN];

    // static int Constructor(lua_State* L)
    // {
    // 	//use userdata for lua gc

    // 	//todo:__newindex to add filed outside

    // 	void* addr = lua_newuserdata(L, sizeof(T));
    // 	new (addr) T;

    // 	luaL_getmetatable(L, ClassMetatableName);

    // 	lua_setmetatable(L, -2);

    // 	return 1;
    // }
};
template<typename T>
std::vector<typename fsLuaCPP<T>::cpp_func> fsLuaCPP<T>::Funcs;

template<typename T>
char fsLuaCPP<T>::ClassMetatableName[fs_LC_MAX_METATABLENAME_LEN];



// //private constructor
// template<typename T>
// class fsLuaCPP_PrvCns
// {
// 	typedef int (T::*cpp_func)(lua_State* L);

// 	static std::vector<cpp_func> Funcs;

// public:
// 	struct fs_LC_Func
// 	{
// 		inline fs_LC_Func(const char* name, cpp_func func) :Name(name), Func(func) {}
// 		const char* Name;
// 		cpp_func Func;

// 	};


// public:
// 	static void Register(lua_State* L, const char* name, const std::vector<fs_LC_Func>& funcs)
// 	{
// 		//create metatable

// 		//strcpy(ClassName, name);

// 		char mt[64] = { '\0' };
// 		sprintf(mt, "%s_mt", name);

// 		if (luaL_newmetatable(L, mt) != 1)
// 		{
// 			printf("luaL_newmetatable_err:%s", mt);
// 			return;
// 		}
// 		lua_pushstring(L, "__index");
// 		lua_pushvalue(L, -2);
// 		lua_settable(L, -3);

// 		//member function
// 		for (int i = 0; i < funcs.size(); i++)
// 		{
// 			Funcs.push_back(funcs[i].Func);
// 			//push the index in to stack
// 			//int index = Funcs.size() - 1;
// 			lua_pushnumber(L, i);

// 			//associate index with the c func
// 			lua_pushcclosure(L, Func, 1);
// 			lua_setfield(L, -2, funcs[i].Name);
// 		}

// 		//set instance to it's metatable
// 		//lua_pushlightuserdata(L, (void*)&(T::Instance));
// 		//lua_setfield(L, -2, "Instance");

// 		//create new table
// 		lua_newtable(L);

// 		//set metatable
// 		lua_pushvalue(L, -2);
// 		lua_setmetatable(L, -2);

// 		//set to global
// 		lua_setglobal(L, name);
// 	}

// 	static int Func(lua_State* L)
// 	{
// 		int index = lua_tonumber(L, lua_upvalueindex(1));
// 		T** ud = reinterpret_cast<T**>(lua_touserdata(L, 1));

// 		if (ud == nullptr)
// 		{
// 			printf("Func ud == nullptr");
// 			return 0;
// 		}
// 		else
// 		//first parameter will always be the instance, and it's a pointer to pointer
// 			return ((*ud)->*(Funcs[index]))(L);
// 	}

// 	static inline void RegisterBase(lua_State*L, const char* classNameMT, const char* baseNameMT)
// 	{
// 		lua_getfield(L, LUA_REGISTRYINDEX, classNameMT);
// 		if (!lua_istable(L, -1))
// 		{
// 			printf("fsLuaCPP_PrvCns derived mt not found.");
// 			return;
// 		}
// 		lua_getfield(L, LUA_REGISTRYINDEX, baseNameMT);
// 		if (!lua_istable(L, -1))
// 		{
// 			printf("fsLuaCPP_PrvCns base mt not found.");
// 			return;
// 		}
// 		lua_setmetatable(L, -2);
// 	}
// 	//static char ClassName[64];
// };
// template<typename T>
// std::vector<typename fsLuaCPP_PrvCns<T>::cpp_func> fsLuaCPP_PrvCns<T>::Funcs;

//template<typename T>
//char fsLuaCPP_Singleton<T>::ClassName[64];

/**
   c struct <<===>> lua table
*/
#define fs_LC_lt2st_begin					\
    public:							\
    inline bool lt2st(lua_State* L, const unsigned int tblIdx)	\
    {								\
	if(lua_type(L, tblIdx) == LUA_TTABLE)			\
	{

#define fs_LC_lt2st_end				\
    return true;				\
    }						\
	else					\
	{					\
	    return false;			\
	}					\
	}

#define fs_LC_lt2st_string(val_name)		\
    lua_getfield(L, tblIdx, #val_name);		\
    if(lua_type(L, -1) == LUA_TSTRING)		\
    {						\
	val_name = lua_tostring(L, -1);		\
	lua_pop(L, 1);				\
    }						\
    else					\
	return false;				

#define fs_LC_lt2st_integer(val_name)		\
    lua_getfield(L, tblIdx, #val_name);		\
    if(lua_type(L, -1) == LUA_TNUMBER)		\
    {						\
	val_name = lua_tointeger(L, -1);	\
	lua_pop(L, 1);				\
    }						\
    else					\
	return false;				

#define fs_LC_st2lt_begin			\
    public:					\
    inline bool st2lt(lua_State*L) const	\
    {						\
	lua_newtable(L);			\
	if(lua_type(L, -1) == LUA_TTABLE)	\
	{ 

#define fs_LC_st2lt_end				\
    return true;				\
    }						\
	else					\
	{					\
	    return false;			\
	}					\
	}

#define fs_LC_st2lt_string(val_name)		\
    lua_pushstring(L, val_name.c_str());	\
    lua_setfield(L, -2, #val_name);

#define fs_LC_st2lt_interger(val_name)		\
    lua_pushinteger(L, val_name);		\
    lua_setfield(L, -2, #val_name);

#endif
