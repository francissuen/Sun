#pragma once
#include "ns.h"
#include "string.h"
#include "luastate.h"
#include <vector>

#include <functional>

#define _GB_UTILS_LUATABLE_MAPPER_GETTER_DECL(ret_type, name)		\
    ret_type get_##name##_by_key(const char* key) const;		\
    ret_type get_##name##_by_idx(const size_t idx) const;		\
    std::vector<ret_type> get_##name##s() const;			\
    std::vector<ret_type> get_##name##s_by_key(const char* key) const;	\
    std::vector<ret_type> get_##name##s_by_idx(const size_t idx) const;	\

GB_UTILS_NS_BEGIN

class luatable_mapper;

//struct luatable
//{
//    inline virtual ~luatable() {}
//    virtual void from_lua(const luatable_mapper& mapper) = 0;
//    inline virtual void to_lua(const
//#if defined(__GNUC__) || defined(__clang__)
//			       __attribute__((unused))
//#endif
//			       luatable_mapper& mapper) const {}
//
//    
//};

class luatable_mapper
{
public:
    luatable_mapper(luastate& ls);
    luatable_mapper(const char* file, luastate& ls);
    ~luatable_mapper();
public:
    bool validate();
    size_t objlen()const;
    bool has_key(const char* key)const;
    _GB_UTILS_LUATABLE_MAPPER_GETTER_DECL(lua_Number, number);
    _GB_UTILS_LUATABLE_MAPPER_GETTER_DECL(lua_Integer, integer);
    _GB_UTILS_LUATABLE_MAPPER_GETTER_DECL(gb::utils::string, string);

    template <typename Table>
    Table get_table() const
	{
	    Table table;
	    if(lua_type(_l, -1) == LUA_TTABLE)
		table.from_lua(*this);
	    return table;
	}

    template <typename Table>
    Table get_table_by_key(const char* key) const
	{
	    GB_ASSERT(key != nullptr);
	    lua_getfield(_l, -1, key);
	    Table table;
	    if(lua_type(_l, -1) == LUA_TTABLE)
		table.from_lua(*this);
	    lua_pop(_l, 1);
	    return table;
	}
    template<typename Table>
    Table get_table_by_idx(const size_t idx) const
	{
	    GB_ASSERT( idx>= 1);
	    lua_rawgeti(_l, -1, idx);
	    Table table;
	    if(lua_type(_l, -1) == LUA_TTABLE)
		table.from_lua(*this);
	    lua_pop(_l, 1);
	    return table;	    
	}
    template<typename Table>
    std::vector<Table> get_tables_by_key(const char* key) const
	{
	    GB_ASSERT(key != nullptr);
	    lua_getfield(_l, -1, key);
	    std::vector<Table> ret;
	    if(lua_type(_l, -1) == LUA_TTABLE)
	    {
		const std::size_t len = lua_objlen(_l, -1);
		ret.reserve(len);
		for(std::size_t i = 1; i <= len; i++)
		{
		    ret.push_back(get_table_by_idx<Table>(i));
		}
	    }
	    lua_pop(_l, 1);
	    return ret;
	}
    template<typename Table>
    std::vector<Table> get_tables_by_idx(const size_t idx) const
	{
	    GB_ASSERT( idx>= 1);
	    lua_rawgeti(_l, -1, idx);
	    std::vector<Table> ret;
	    if(lua_type(_l, -1) == LUA_TTABLE)
	    {
		const std::size_t len = lua_objlen(_l, -1);
		ret.reserve(len);
		for(std::size_t i = 1; i <= len; i++)
		{
		    ret.push_back(get_table_by_idx<Table>(i));
		}
	    }
	    lua_pop(_l, 1);
	    return ret;
	}
    
    void for_each(const std::function<void(const std::size_t)>& func) const;
    void for_each_in(const std::function<void(const std::size_t)>& func, const char* key) const;
    void for_each_in(const std::function<void(const std::size_t)>& func, const std::size_t idx) const;
    // scene::instantiate(const char* luafile)
    // 	{
    // 	    luatable_mapper lt(luafile);
    // 	    lt.get_cpps([](lua_State* l)
    // 			{
    // 			    Entity e;
    // 			    e.instantiate(l);//may also instantiate(luafile)//prefab instantiate
    // 			    _mpEntitys.insert(e.name, e);
    // 			});
    // 	}
    // void get_cpps(func)
    // 	{
    // 	    size_t len = lua_objlen(_l, -1);
    // 	    for(size_t i = 1; i <= len; i++)
    // 	    {
    // 		lua_rawgeti(_l, -1, i);
    // 		func(_l);

    // 		lua_pop(_l, 1);
    // 	    }
    // 	}
private:
    lua_State* _l;
    GB_PROPERTY_R(private, File, gb::utils::string);
    GB_PROPERTY_R(private, Validated, bool);
};

GB_UTILS_NS_END
