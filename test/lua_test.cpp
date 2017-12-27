extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
}
int main(int argc, char** argv)
{
    lua_State* l = luaL_newstate();
    
    return l != nullptr? 0 : 1;
}
