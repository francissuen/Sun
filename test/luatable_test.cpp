#include "../src/luatable.h"
#include "../src/string.h"
#include "../src/filesystem.h"
#include <vector>
#include <algorithm>
using namespace gb::utils;


struct luatable_0
{
    luatable_0(){}
    luatable_0(luatable* tbl)
	{
	    assert(tbl != nullptr);
	    if(tbl->validate())
	    {
		a = tbl->get_integer_by_key("a");
		b = tbl->get_number_by_key("b");
		c = tbl->get_string_by_key("c");
	    }
	}
    void print()
	{
	logger::Instance().log(string("luatable_0:") + "a:" + a +
			       ", b:" + b +
			       ", c:" + c);
	}
    int a;
    float b;
    string c;
};

struct luatable_1
{
public:
    luatable_1(){}
    luatable_1(luatable* tbl)
	{
	    assert(tbl != nullptr);
	    vTbl0 = tbl->get_tables<luatable_0>();
	}
    void print()
	{
	    std::for_each(vTbl0.begin(), vTbl0.end(), [](luatable_0& t)
			  {
			      t.print();
			  });
	}
    std::vector<luatable_0> vTbl0;
};

int luatable_test()
{
    luatable tl("../luatable_0", luastate_mgr::Instance().getconfig_state());
    if(tl.validate())
    {
	luatable_0 lt0(&tl);
	lt0.print();

	luatable tl1("../luatable_1", luastate_mgr::Instance().getconfig_state());
	if(tl1.validate())
	{
	    luatable_1 lt1(&tl1);
	    lt1.print();
	}
	else
	    return 1;
    }
    else
	return 1;
    return 0;
}
