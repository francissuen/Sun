#pragma once
#include "ns.h"
#include "string.h"
#include "luastate.h"
#include <vector>

#include <functional>

/*
 * get* return a default value when not exist.
 * checkout* set out_val only when exists
 */

#define _GB_UTILS_LUATABLE_MAPPER_GETTER_DECL(ret_type, name)		\
    ret_type get_##name##_by_key(const char* key) const;		\
    ret_type get_##name##_by_idx(const size_t idx) const;		\
    std::vector<ret_type> get_##name##s() const;			\
    std::vector<ret_type> get_##name##s_by_key(const char* key) const;	\
    std::vector<ret_type> get_##name##s_by_idx(const size_t idx) const;	\
    void checkout_##name##_by_key(const char* key, ret_type* out_val) const; \
    void checkout_##name##_by_idx(const size_t idx, ret_type* out_val) const; \
    void checkout_##name##s(std::vector<ret_type>& out_val) const;	\
    void checkout_##name##s_by_key(const char* key, std::vector<ret_type>& out_val) const; \
    void checkout_##name##s_by_idx(const size_t idx, std::vector<ret_type>& out_val) const;

GB_UTILS_NS_BEGIN

class luatable_mapper
{
public:
    luatable_mapper(luastate& ls);
    ~luatable_mapper();
public:
    bool map_file(const char* file);
    bool map_string(const char* luaCode);
    void unmap();
    bool load_table(const char* key);
    void unload_table();
    size_t objlen()const;
    bool has_key(const char* key)const;
    _GB_UTILS_LUATABLE_MAPPER_GETTER_DECL(lua_Number, number);
    _GB_UTILS_LUATABLE_MAPPER_GETTER_DECL(lua_Integer, integer);
    _GB_UTILS_LUATABLE_MAPPER_GETTER_DECL(gb::utils::string, string);
    _GB_UTILS_LUATABLE_MAPPER_GETTER_DECL(bool, boolean);
    template <typename Table>
    Table get_table() const
    {
	Table table;
	if(lua_type(_l, -1) == LUA_TTABLE)
	    table.from_lua(*this);
	return table;
    }

    template <typename Table>
    void checkout_table(Table& out_val) const
    {
	if(lua_type(_l, -1) == LUA_TTABLE)
	    out_val.from_lua(*this);
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
    template <typename Table>
    void checkout_table_by_key(const char* key, Table& out_val) const
    {
	GB_ASSERT(key != nullptr);
	lua_getfield(_l, -1, key);
	if(lua_type(_l, -1) == LUA_TTABLE)
	    out_val.from_lua(*this);
	lua_pop(_l, 1);
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
    void checkout_table_by_idx(const size_t idx, Table& out_val) const
    {
	GB_ASSERT( idx>= 1);
	lua_rawgeti(_l, -1, idx);
	if(lua_type(_l, -1) == LUA_TTABLE)
	    out_val.from_lua(*this);
	lua_pop(_l, 1);
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
    void checkout_tables_by_key(const char* key, std::vector<Table>& out_val) const
    {
	GB_ASSERT(key != nullptr);
	lua_getfield(_l, -1, key);
	if(lua_type(_l, -1) == LUA_TTABLE)
	    {
		const std::size_t len = lua_objlen(_l, -1);
		out_val.clear();
		out_val.reserve(len);
		for(std::size_t i = 1; i <= len; i++)
		    {
			out_val.push_back(get_table_by_idx<Table>(i));
		    }
	    }
	lua_pop(_l, 1);
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
    template<typename Table>
    void checkout_tables_by_idx(const size_t idx, std::vector<Table>& out_val) const
    {
	GB_ASSERT( idx>= 1);
	lua_rawgeti(_l, -1, idx);
	if(lua_type(_l, -1) == LUA_TTABLE)
	    {
		const std::size_t len = lua_objlen(_l, -1);
		out_val.clear();
		out_val.reserve(len);
		for(std::size_t i = 1; i <= len; i++)
		    {
			out_val.push_back(get_table_by_idx<Table>(i));
		    }
	    }
	lua_pop(_l, 1);
    }
    
    void for_each(const std::function<void(const std::size_t)>& func) const;
    void for_each_in(const std::function<void(const std::size_t)>& func, const char* key) const;
    void for_each_in(const std::function<void(const std::size_t)>& func, const std::size_t idx) const;
private:
    
    lua_State* _l;
    luastate& _ls;
    
    GB_PROPERTY_R(private, Data, gb::utils::string);
    GB_PROPERTY_R(private, UnbalancedStack, int);

private:
    void _balanceStack();

private:
    bool _validate();
};

GB_UTILS_NS_END
