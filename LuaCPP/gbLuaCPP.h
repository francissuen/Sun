#pragma once
#include "../gbUtilsConfig.h"

#define gbLUAAPI 1


//lua 5.1 reference manual:http://www.lua.org/manual/5.1/manual.html

#ifdef gbLUAAPI

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

//todo, re-link lua lib in other project issue

#include <vector>
#include <cstring>

#define gbLC_METATABLE_SUFFIX "_mt"
#define MTNAME(type) #type gbLC_METATABLE_SUFFIX
#define gbUSERDATA "gb_userdata"
#endif

#ifdef gbLUAAPI
#define gb_LC_Register(ClassName)				\
  private:							\
  typedef gbLuaCPP<ClassName>::gb_LC_Func gb_LC_Func;		\
public:								\
 static void gb_LC_Reg(lua_State* L);				
//inline int gb_LC_EF_Destructor(lua_State* L){delete this;}
#else
#define gb_LC_Register(ClassName)
#endif

#ifdef gbLUAAPI
#define gb_LC_Register_PrvCns(ClassName) \
  private: \
  typedef gbLuaCPP_PrvCns<ClassName>::gb_LC_Func gb_LC_Func; \
public: \
 static void gb_LC_Reg(lua_State* L);	
#else
#define gb_LC_Register_PrvCns(ClassName)
#endif


//used after register
#ifdef gbLUAAPI
#define gb_LC_Singleton_Instance_Reg(ClassName) \
luaL_getmetatable(L, MTNAME(ClassName)); \
void* p = lua_newuserdata(L, sizeof(void*)); \
*(unsigned int*)p = (unsigned int)&(ClassName::Instance); \
lua_pushvalue(L, -2); \
lua_setmetatable(L, -2);\
lua_setfield(L, -2, "Instance");
#else
#define gb_LC_Singleton_Instance_Reg(ClassName)
#endif

#ifdef gbLUAAPI
//actual pointer wrapped by a data which point to the pointer
inline int gb_LC_push_userdata(lua_State* L, unsigned int ud, const char* mtName)
{
	if (luaL_newmetatable(L, "gb_userdata") != 0) 
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

#ifdef gbLUAAPI
#define gb_LC_EXPORT_INLINE_FUNC_1R_LUD_0P(ret, name, definition) \
public: \
inline ret* name() definition \
private: \
	inline int gb_LC_EF_##name(lua_State* L) \
	{ \
        unsigned int ud = (unsigned int)(name()); \
        return gb_LC_push_userdata(L, ud, MTNAME(ret)); \
	}
#else
#define gb_LC_EXPORT_INLINE_FUNC_1R_LUD_0P(ret, name, definition) \
public: \
inline ret* name() definition 
#endif

#ifdef gbLUAAPI
#define gb_LC_EXPORT_INLINE_FUNC_1R_0P(ret, ret_pusher, name, definition) \
public: \
inline ret name() definition \
private: \
	inline int gb_LC_EF_##name(lua_State* L) \
	{ \
		ret_pusher(L, name());\
		return 1;\
	}
#else
#define gb_LC_EXPORT_INLINE_FUNC_1R_0P(ret, ret_pusher, name, definition) \
public: \
inline ret name() definition 
#endif

#ifdef gbLUAAPI
#define gb_LC_EXPORT_FUNC_1R_0P(ret, ret_pusher, name) \
public: \
ret name(); \
private: \
	inline int gb_LC_EF_##name(lua_State* L) \
	{ \
		ret_pusher(L, name());\
		return 1;\
	}
#else
#define gb_LC_EXPORT_FUNC_1R_0P(ret, ret_pusher, name) \
public: \
ret name(); 
#endif

#ifdef gbLUAAPI
#define gb_LC_EXPORT_INLINE_FUNC_0R_1P_TABLE(name, tp, p_name, p_getter, definition) \
public: \
inline void name(tp& p_name) definition \
private: \
inline int gb_LC_EF_##name(lua_State* L) \
	{ \
		tp arg; \
		p_getter(L, 2, arg); \
		name(arg); \
		return 0; \
	}
#else
#define gb_LC_EXPORT_INLINE_FUNC_0R_1P_TABLE(name, tp, p_name, p_getter, definition) \
public: \
	inline void name(tp p_name) definition 
#endif

#ifdef gbLUAAPI
#define gb_LC_EXPORT_FUNC_0R_2P(name, tp1, p1_getter, tp2, p2_getter) \
public: \
void name(tp1, tp2); \
private: \
	inline int gb_LC_EF_##name(lua_State* L) \
	{ \
		tp1 p1 = p1_getter(L, 2); \
		tp2 p2 = p2_getter(L, 3); \
		name(p1, p2);\
		return 0;\
	}
#else
#define gb_LC_EXPORT_FUNC_0R_2P(name, tp1, p1_getter, tp2, p2_getter) \
public: \
void name(tp1, tp2); 
#endif

#ifdef gbLUAAPI
#define gb_LC_EXPORT_FUNC_0R_0P(name) \
public: \
void name(); \
private: \
	inline int gb_LC_EF_##name(lua_State* L) \
	{ \
		name(); \
		return 0;\
	}
#else
#define gb_LC_EXPORT_FUNC_0R_0P(name) \
public: \
void name(); 
#endif

#ifdef gbLUAAPI
#define gb_LC_EXPORT_INLINE_FUNC_0R_0P(name, definition) \
public: \
inline void name() definition \
private: \
	inline int gb_LC_EF_##name(lua_State* L) \
	{ \
		name(); \
		return 0;\
	}
#else
#define gb_LC_EXPORT_INLINE_FUNC_0R_0P(name, definition) \
public: \
inline void name() definition 
#endif

#ifdef gbLUAAPI
#define gb_LC_EXPORT_INLINE_FUNC_0R_1P(name, tp, p, definition, p_getter) \
public: \
void name(tp p) definition \
private: \
	inline int gb_LC_EF_##name(lua_State* L) \
	{ \
		tp arg = (tp)p_getter(L, 2); \
		name(arg); \
		return 0;\
	}
#else
#define gb_LC_EXPORT_INLINE_FUNC_0R_1P(name, tp, p, definition, p_getter) \
public: \
void name(); 
#endif

#ifdef gbLUAAPI
#define gb_LC_EXPORT_FUNC_0R_1P(name, tp, p_getter) \
public: \
void name(tp); \
private: \
	inline int gb_LC_EF_##name(lua_State* L) \
	{ \
		tp arg = p_getter(L, 2); \
		name(arg); \
		return 0;\
	}
#else
#define gb_LC_EXPORT_FUNC_0R_0P(name) \
public: \
void name(); 
#endif

#ifdef gbLUAAPI
#define gb_LC_EXPORT_FUNC_1R_1P(ret, ret_pusher, name, arg_type, arg_geter)	\
  public:							\
  ret name(arg_type);						\
private:							\
 inline int gb_LC_EF_##name(lua_State* L)			\
 {								\
   arg_type arg = arg_geter(L, 2);				\
   ret_pusher(L, name(arg));\						\
   return 1;							\
 }
#else
#define gb_LC_EXPORT_FUNC_1R_1P(ret, ret_pusher, name, arg_type, arg_geter)	\
  public:							\
  ret name(arg_type);
#endif

#ifdef gbLUAAPI
#define gb_LC_EXPORT_FUNC_1R_LUD_1P(ret, name, arg_type, arg_geter)	\
public: \
	ret* name(arg_type); \
private: \
	inline int gb_LC_EF_##name(lua_State* L) \
	{ \
		arg_type arg = arg_geter(L, 2);	\
		unsigned int ud = (unsigned int)(name(arg)); \
		return gb_LC_push_userdata(L, ud, MTNAME(ret)); \
	}
#else
#define gb_LC_EXPORT_FUNC_1R_LUD_1P(ret, name, arg_type, arg_geter)	\
public: \
	ret* name(arg_type); 
#endif

#ifdef gbLUAAPI
#define gb_LC_EXPORT_INLINE_FUNC_1R_LUD_1P(ret, name, definition, param, arg_type, arg_geter)	\
public: \
	inline ret* name(arg_type param) definition \
private: \
	inline int gb_LC_EF_##name(lua_State* L) \
	{ \
		arg_type arg = arg_geter(L, 2);	\
		unsigned int ud = (unsigned int)(name(arg)); \
		return gb_LC_push_userdata(L, ud, MTNAME(ret)); \
	}
#else
#define gb_LC_EXPORT_INLINE_FUNC_1R_LUD_1P(ret, name, definition, arg_type, arg_geter)	\
public: \
	inline ret* name(arg_type param) definition 
#endif

#ifdef gbLUAAPI
#define gb_LC_MKFUNCNAME(name) gb_LC_EF_##name
#else
#define gb_LC_MKFUNCNAME(name) name
#endif


#ifdef gbLUAAPI
#define gb_LC_EXPORT_FUNC(name) int gb_LC_EF_##name(lua_State* L)
#else
#define gb_LC_EXPORT_FUNC(name)
#endif

#ifdef gbLUAAPI
#define gb_LC_EXPORT_WRAP_FUNC(name, definition) \
private: \
	inline int gb_LC_EF_##name(lua_State* L) definition
#else
#define gb_LC_EXPORT_WRAP_FUNC(name, definition) 
#endif

#ifdef gbLUAAPI
//read only
#define gb_LC_EXPORT_VAR_R(type, name, push_method)	\
  public:						\
  type name;						\
private:						\
 inline int gb_LC_EV_R_##name(lua_State* L)		\
 {							\
   push_method(L, name);				\
   return 1;						\
 }
#else
#define gb_LC_EXPORT_VAR_R(type, name, push_method)	\
  public:						\
  type name;
#endif

#ifdef gbLUAAPI
//read&write
#define gb_LC_EXPORT_VAR_RW(type, name, push_method, pop_method)	\
  public:								\
  type name;								\
private:								\
 inline int gb_LC_EV_R_##name(lua_State* L)				\
 {									\
   push_method(L, name);						\
   return 1;								\
 }									\
 inline int gb_LC_EV_W_##name(lua_State* L)				\
 {									\
   name = pop_method(L, 1);						\
   return 1;								\
 }
#else
#define gb_LC_EXPORT_VAR_RW(type, name, push_method, pop_method)	\
  public:								\
  type name;	
#endif

#ifdef gbLUAAPI

//#ifdef gbUtilsSTATIC
//extern lua_State* gbLuaState;
//#else
//#ifdef gbUtilsEXPORT
//extern _declspec(dllexport)lua_State* gbLuaState;
//#else
//extern _declspec(dllimport)lua_State* gbLuaState;
//#endif
//#endif

#ifdef gbUtilsSTATIC
int gbLuaTraceback(lua_State* L);
#else
#ifdef gbUtilsEXPORT
_declspec(dllexport) int gbLuaTraceback(lua_State* L);
#else
_declspec(dllimport) int gbLuaTraceback(lua_State* L);
#endif
#endif

#ifdef gbUtilsSTATIC
lua_State* gbLuaCPP_init();
#else
#ifdef gbUtilsEXPORT
_declspec(dllexport) lua_State* gbLuaCPP_init();
#else
_declspec(dllimport) lua_State* gbLuaCPP_init();
#endif
#endif

#ifdef gbUtilsSTATIC
bool gbLuaCPP_dofile(lua_State* L, const char* szLuaFile);
#else
#ifdef gbUtilsEXPORT
_declspec(dllexport) bool gbLuaCPP_dofile(lua_State* L, const char* szLuaFile);
#else
_declspec(dllimport) bool gbLuaCPP_dofile(lua_State* L, const char* szLuaFile);
#endif
#endif

#ifdef gbUtilsSTATIC
bool gbLuaCPP_dostring(lua_State* L, const char* szLua);
#else
#ifdef gbUtilsEXPORT
_declspec(dllexport) bool gbLuaCPP_dostring(lua_State* L, const char* szLua);
#else
_declspec(dllimport) bool gbLuaCPP_dostring(lua_State* L, const char* szLua);
#endif
#endif

#ifdef gbUtilsSTATIC
void gbLuaCPP_appendPackagePath(lua_State* L, const char* path);
#else
#ifdef gbUtilsEXPORT
_declspec(dllexport) void gbLuaCPP_appendPackagePath(lua_State* L, const char* path);
#else
_declspec(dllimport) void gbLuaCPP_appendPackagePath(lua_State* L, const char* path);
#endif
#endif


//#ifdef gbRCDLLEXPORT
//_declspec(dllexport) int gbLuaTraceback(lua_State* L);
//#else
//_declspec(dllimport) int gbLuaTraceback(lua_State* L);
//#endif

#define gb_LC(className) \
className::gb_LC_Reg(L);

#define gb_LC_Func_Def \
std::vector<gb_LC_Func> funcs;

#define gb_LC_Func_push(name, pointer) \
funcs.push_back(gb_LC_Func(name, pointer))

#define gb_LC_CAST_LUD_TO(type, ud) (type)(*(int*)ud)

struct gbLCLuaFunc
{
	inline gbLCLuaFunc() :tblIdx(0), funcIdx(0)/*, argc(1), retc(0)*/ {}
	int tblIdx;
	int funcIdx;
	//unsigned int argc;
	//unsigned int retc;
};

class gbLCString
{
public:
	inline  gbLCString() :_data(nullptr) {}
	inline  gbLCString(const char* val) : _size(strlen(val) + 1), _data(new char[_size])
	{
		memcpy(_data, val, _size - 1);
		_data[_size - 1] = '\0';
	}
	inline ~gbLCString()
	{
		if (_data != nullptr)
		{
			delete[] _data;
			_data = nullptr;
		}
	}
public:
	inline operator const char*()
	{
		return _data;
	}
	inline gbLCString& operator=(const char* val)
	{
		if (_data != nullptr)
		{
			delete[] _data;
			_data = nullptr;
		}
		_size = strlen(val) + 1;
		_data = new char[_size];
		memcpy(_data, val, _size - 1);
		_data[_size - 1] = '\0';
		return *this;
	}
	inline  gbLCString& operator=(const gbLCString& other)
	{
		if (_data != nullptr)
		{
			delete[] _data;
			_data = nullptr;
		}
		_size = other.GetSize();
		_data = new char[_size];
		memcpy(_data, other.GetData(), _size - 1);
		_data[_size - 1] = '\0';
		return *this;
	}
public:
	inline unsigned int GetSize()const { return _size; }
	inline const char* GetData()const { return _data; }
private:
	unsigned int _size;
	char* _data;
};

//inline int gb_LC_CallLuaFunc(lua_State* L, const gbLCLuaFunc& func)
//{
//	lua_rawgeti(L, LUA_REGISTRYINDEX, func.funcIdx);
//	lua_rawgeti(L, LUA_REGISTRYINDEX, func.tblIdx);
//	lua_call(L, func.argc, func.retc);
//	return func.retc;
//}

inline int gb_LC_RegisterLuaFunc(lua_State* L, int& tblIdx, int& funcIdx)
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
struct gb_LC_StaticFunc
{
	inline gb_LC_StaticFunc(const char* name, cpp_static_func func) :Name(name), Func(func) {}
	const char* Name;
	cpp_static_func Func;

};

inline bool gb_LC_RegisterStaticFunc(lua_State* L, const std::vector<gb_LC_StaticFunc>& funcs, const char* ClassMetatableName)
{
	lua_getfield(L, LUA_REGISTRYINDEX, ClassMetatableName);
	if (!lua_istable(L, -1))
	{
		printf("gb_LC_RegisterStateFunc !lua_istable(L, -1)");
		return false;
	}

	for (std::vector<gb_LC_StaticFunc>::const_iterator i = funcs.cbegin(); i != funcs.cend(); i++)
	{
		lua_pushcclosure(L, (*i).Func, 0);
		lua_setfield(L, -2, (*i).Name);
	}
	return true;
}

template<typename T>
class gbLuaCPP
{
	typedef int (T::*cpp_func)(lua_State* L);

	static std::vector<cpp_func> Funcs;

public:
	struct gb_LC_Func
	{
		inline gb_LC_Func(const char* name, cpp_func func) :Name(name), Func(func) {}
		const char* Name;
		cpp_func Func;

	};


public:
	static void Register(lua_State* L, const char* name, const std::vector<gb_LC_Func>& funcs)
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
		lua_pushcfunction(L, &gbLuaCPP::Constructor);
		lua_setglobal(L, name);
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

	static char ClassMetatableName[64];

	static int Constructor(lua_State* L)
	{
		//use userdata for lua gc

		//todo:__newindex to add filed outside

		void* addr = lua_newuserdata(L, sizeof(T));
		new (addr) T;

		luaL_getmetatable(L, ClassMetatableName);

		lua_setmetatable(L, -2);

		return 1;
	}
};
template<typename T>
std::vector<typename gbLuaCPP<T>::cpp_func> gbLuaCPP<T>::Funcs;

template<typename T>
char gbLuaCPP<T>::ClassMetatableName[64];


//private constructor
template<typename T>
class gbLuaCPP_PrvCns
{
	typedef int (T::*cpp_func)(lua_State* L);

	static std::vector<cpp_func> Funcs;

public:
	struct gb_LC_Func
	{
		inline gb_LC_Func(const char* name, cpp_func func) :Name(name), Func(func) {}
		const char* Name;
		cpp_func Func;

	};


public:
	static void Register(lua_State* L, const char* name, const std::vector<gb_LC_Func>& funcs)
	{
		//create metatable

		//strcpy(ClassName, name);

		char mt[64] = { '\0' };
		sprintf(mt, "%s_mt", name);

		if (luaL_newmetatable(L, mt) != 1)
		{
			printf("luaL_newmetatable_err:%s", mt);
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

		//set instance to it's metatable
		//lua_pushlightuserdata(L, (void*)&(T::Instance));
		//lua_setfield(L, -2, "Instance");

		//create new table
		lua_newtable(L);

		//set metatable
		lua_pushvalue(L, -2);
		lua_setmetatable(L, -2);

		//set to global
		lua_setglobal(L, name);
	}

	static int Func(lua_State* L)
	{
		int index = lua_tonumber(L, lua_upvalueindex(1));
		T** ud = reinterpret_cast<T**>(lua_touserdata(L, 1));

		if (ud == nullptr)
		{
			printf("Func ud == nullptr");
			return 0;
		}
		else
		//first parameter will always be the instance, and it's a pointer to pointer
			return ((*ud)->*(Funcs[index]))(L);
	}

	static inline void RegisterBase(lua_State*L, const char* classNameMT, const char* baseNameMT)
	{
		lua_getfield(L, LUA_REGISTRYINDEX, classNameMT);
		if (!lua_istable(L, -1))
		{
			printf("gbLuaCPP_PrvCns derived mt not found.");
			return;
		}
		lua_getfield(L, LUA_REGISTRYINDEX, baseNameMT);
		if (!lua_istable(L, -1))
		{
			printf("gbLuaCPP_PrvCns base mt not found.");
			return;
		}
		lua_setmetatable(L, -2);
	}
	//static char ClassName[64];
};
template<typename T>
std::vector<typename gbLuaCPP_PrvCns<T>::cpp_func> gbLuaCPP_PrvCns<T>::Funcs;

//template<typename T>
//char gbLuaCPP_Singleton<T>::ClassName[64];

#endif
