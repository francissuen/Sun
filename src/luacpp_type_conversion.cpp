#include "luacpp_type_conversion.h"

//std::vector<cpp_static_func> fsLCStaticFuncs;

//template bool fsLCTypeConversion::TableToArray<float>(lua_State* L, const int idx, float* outVal, const unsigned int size);
//template bool fsLCTypeConversion::TableToArray<int>(lua_State* L, const unsigned int idx, int* outVal, const unsigned int size);
//template bool fsLCTypeConversion::TableToArray<unsigned int>(lua_State* L, const unsigned int idx, unsigned int* outVal, const unsigned int size);

template int fsLCTypeConversion::ArrayToTable<float>(lua_State* L, const float* arr, const unsigned int size);
//template int fsLCTypeConversion::ArrayToTable<int>(lua_State* L, const int* arr, const unsigned int size);
//template int fsLCTypeConversion::ArrayToTable<unsigned int>(lua_State* L, const unsigned int* arr, const unsigned int size);

fsLCTypeConversion::fsLCTypeConversion()
{
}


fsLCTypeConversion::~fsLCTypeConversion()
{
}


//template<typename T>
//bool fsLCTypeConversion::TableToArray(lua_State* L, const unsigned int idx, T* outVal, const unsigned int count)
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

bool fsLCTypeConversion::TableToArrayInt(lua_State* L, const int idx, lua_Integer* outVal, const unsigned int count)
{
	if (!lua_istable(L, idx))
	{
		printf("fsLCTypeConversion::TableToArrayInt:(!lua_istable(L, idx))");
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
			printf("fsLCTypeConversion::TableToArrayInt:not a number@%d, assigned to 0", i);
			outVal[i] = 0;
		}
		lua_pop(L, 1);
	}
	return true;
}

bool fsLCTypeConversion::TableToArrayFloat(lua_State* L, const int idx, float* outVal, const unsigned int count)
{
	if (!lua_istable(L, idx))
	{
		printf("fsLCTypeConversion::TableToArray:(!lua_istable(L, idx))");
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
			printf("fsLCTypeConversion::TableToArray:not a number@%d, assigned to 0.0f", i);
			outVal[i] = 0.0f;
		}
		lua_pop(L, 1);
	}
	return true;
}


bool fsLCTypeConversion::TableToVec3(lua_State* L, const int idx, float (&outVal)[3])
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

int fsLCTypeConversion::Vec3ToTable(lua_State* L, const float  (&inVal)[3])
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
int fsLCTypeConversion::ArrayToTable(lua_State* L, const T* arr, const unsigned int count)
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
//int fsLCTypeConversion::ArrayToTable(lua_State* L, const lua_Number* arr, const unsigned int count)
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
int fsLCTypeConversion::ArrayIntToTable(lua_State* L, const lua_Integer* arr, const unsigned int count)
{
	lua_newtable(L);

	for (unsigned int i = 0; i < count; i++)
	{
		lua_pushinteger(L, arr[i]);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

