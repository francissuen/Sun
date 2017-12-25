#include "../src/luatable.h"
#include "../src/string.h"
#include "../src/filesystem.h"
#include <vector>
using namespace gb::utils;


class luatable_0
{
public:
    void parse(const char* file)
	{
	    string path = filesystem::Instance().get_absolute_path(file);
	    luatable tbl(path, luastate_mgr::Instance().getconfig_state());
	    if(tbl.validate())
	    {
		a = tbl.get_integer_by_key("a");
		b = tbl.get_number_by_key("b");
		c = tbl.get_string_by_key("c");
	    }
	}

    void parse(luatable& tbl)
	{
	    
	}
private:
    int a;
    float b;
    string c;
};

class luatable_1
{
    void parse(const char* file)
	{
	    string path = filesystem::Instance().get_absolute_path(file);
	    luatable tbl(path, luastate_mgr::Instance().getconfig_state());
	    if(tbl.validate())
	    {
		const size_t len = tbl.objlen();
		for(size_t i = 1; i <= len; i ++)
		{
		    luatable_0 tbl0;
		    if(tbl.get_table_by_idx(i))
		    {
			tbl0.parse(tbl);
			vTbl0.push_back(tbl0);
		    }

//		    lua_pop();
		}
	    }
	}
private:
    std::vector<luatable_0> vTbl0;
};
