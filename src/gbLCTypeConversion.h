#pragma once
#include "gbLuaCPP.h"
//#include "../gbUtils/Math/gbMath.h"
class gbLCTypeConversion
{
public:
	gbLCTypeConversion();
	~gbLCTypeConversion();


	//lua table conversion

	static bool TableToArrayFloat(lua_State* L, const int idx, float* outVal, const unsigned int count);
	//static bool TableToArray(lua_State* L, const int idx, lua_Number* outVal, const unsigned int count);
	static bool TableToArrayInt(lua_State* L, const int idx, lua_Integer* outVal, const unsigned int count);
	//static bool TableToArrayInt(lua_State* L, const int idx, std::vector<lua_Integer>& outVal, const unsigned int count);

	static bool TableToVec3(lua_State* L, const int idx, float (&outVal)[3]);

	static int Vec3ToTable(lua_State* L, const float (&inVal)[3]);

	template<typename T>
	static int ArrayToTable(lua_State* L, const T* arr, const unsigned int count);
	static int ArrayIntToTable(lua_State* L, const lua_Integer* arr, const unsigned int count);

};


