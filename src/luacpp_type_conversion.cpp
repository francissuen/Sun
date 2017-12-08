#include "luacpp_type_conversion.h"

//std::vector<cpp_static_func> gbLCStaticFuncs;

//template bool gbLCTypeConversion::TableToArray<float>(lua_State* L, const int idx, float* outVal, const unsigned int size);
//template bool gbLCTypeConversion::TableToArray<int>(lua_State* L, const unsigned int idx, int* outVal, const unsigned int size);
//template bool gbLCTypeConversion::TableToArray<unsigned int>(lua_State* L, const unsigned int idx, unsigned int* outVal, const unsigned int size);

template int gbLCTypeConversion::ArrayToTable<float>(lua_State* L, const float* arr, const unsigned int size);
//template int gbLCTypeConversion::ArrayToTable<int>(lua_State* L, const int* arr, const unsigned int size);
//template int gbLCTypeConversion::ArrayToTable<unsigned int>(lua_State* L, const unsigned int* arr, const unsigned int size);

gbLCTypeConversion::gbLCTypeConversion()
{
}


gbLCTypeConversion::~gbLCTypeConversion()
{
}


//template<typename T>
//bool gbLCTypeConversion::TableToArray(lua_State* L, const unsigned int idx, T* outVal, const unsigned int count)
//{
//	if (!lua_istable(L, idx))
//		return false;
//
//	//if(size > lua_objlen(L, idx);
//	for (int i = 0; i < count; i++)
//	{
//		lua_rawgeti(L, idx, i + 1 /*lua table start at 1*/);
//		outVal[i] = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : 0.0f;
//		lua_pop(L, 1);
//	}
//	return true;
//}

bool gbLCTypeConversion::TableToArrayInt(lua_State* L, const int idx, lua_Integer* outVal, const unsigned int count)
{
	if (!lua_istable(L, idx))
	{
		printf("gbLCTypeConversion::TableToArrayInt:(!lua_istable(L, idx))");
		return false;
	}
	//if(size > lua_objlen(L, idx);
	for (unsigned int i = 0; i < count; i++)
	{
		lua_rawgeti(L, idx, i + 1 /*lua table start at 1*/);
		if (lua_isnumber(L, -1))
			outVal[i] = lua_tointeger(L, -1);
		else
		{
			printf("gbLCTypeConversion::TableToArrayInt:not a number@%d, assigned to 0", i);
			outVal[i] = 0;
		}
		lua_pop(L, 1);
	}
	return true;
}

bool gbLCTypeConversion::TableToArrayFloat(lua_State* L, const int idx, float* outVal, const unsigned int count)
{
	if (!lua_istable(L, idx))
	{
		printf("gbLCTypeConversion::TableToArray:(!lua_istable(L, idx))");
		return false;
	}


	//if(size > lua_objlen(L, idx);
	for (unsigned int i = 0; i < count; i++)
	{
		lua_rawgeti(L, idx, i + 1 /*lua table start at 1*/);
		if (lua_isnumber(L, -1))
			outVal[i] = (float)lua_tonumber(L, -1);
		else
		{
			printf("gbLCTypeConversion::TableToArray:not a number@%d, assigned to 0.0f", i);
			outVal[i] = 0.0f;
		}
		lua_pop(L, 1);
	}
	return true;
}


bool gbLCTypeConversion::TableToVec3(lua_State* L, const int idx, float (&outVal)[3])
{
	if (!lua_istable(L, idx))
		return false;
	lua_getfield(L, idx, "x");
	outVal[0] = (float)lua_isnumber(L, -1) ? (float)lua_tonumber(L, -1) : 0.0f;
	lua_pop(L, 1);

	lua_getfield(L, idx, "y");
	outVal[1] = (float)lua_isnumber(L, -1) ? (float)lua_tonumber(L, -1) : 0.0f;
	lua_pop(L, 1);

	lua_getfield(L, idx, "z");
	outVal[2] = (float)lua_isnumber(L, -1) ? (float)lua_tonumber(L, -1) : 0.0f;
	lua_pop(L, 1);

	return true;
}

int gbLCTypeConversion::Vec3ToTable(lua_State* L, const float  (&inVal)[3])
{
	lua_newtable(L);
	lua_pushnumber(L, inVal[0]);
	lua_setfield(L, -2, "x");

	lua_pushnumber(L, inVal[1]);
	lua_setfield(L, -2, "y");

	lua_pushnumber(L, inVal[2]);
	lua_setfield(L, -2, "z");

	return 1;
}

template<typename T>
int gbLCTypeConversion::ArrayToTable(lua_State* L, const T* arr, const unsigned int count)
{
	lua_newtable(L);
	
	for (unsigned int i = 0; i < count; i++)
	{
		lua_pushnumber(L, arr[i]);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}
//
//int gbLCTypeConversion::ArrayToTable(lua_State* L, const lua_Number* arr, const unsigned int count)
//{
//	lua_newtable(L);
//
//	for (int i = 0; i < count; i++)
//	{
//		lua_pushnumber(L, arr[i]);
//		lua_rawseti(L, -2, i + 1);
//	}
//	return 1;
//}
int gbLCTypeConversion::ArrayIntToTable(lua_State* L, const lua_Integer* arr, const unsigned int count)
{
	lua_newtable(L);

	for (unsigned int i = 0; i < count; i++)
	{
		lua_pushinteger(L, arr[i]);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

