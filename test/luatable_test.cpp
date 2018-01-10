#include "../src/luatable.h"
#include "../src/string.h"
#include "../src/filesystem.h"
#include <map>
#include <algorithm>
using namespace gb::utils;


struct luatable_0
{
	luatable_0() {}
	luatable_0(luatable_0&& o) :
		a(o.a),
		b(o.b),
		c(std::move(o.c))
	{

	}
	void from_lua(const luatable_mapper& mapper)
	{
		if (mapper.validate())
		{
			a = mapper.get_integer_by_key("a");
			b = (float)mapper.get_number_by_key("b");
			c = mapper.get_string_by_key("c");
		}
	}
	void print() const
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
	~luatable_1()
	{
		std::for_each(tbl0s.begin(), tbl0s.end(), [](std::pair<string, luatable_0*>&& t)
		{
			GB_SAFE_DELETE(t.second);
		});
	}
	void from_lua(const luatable_mapper& mapper)
	{
		mapper.for_each([this, mapper](const size_t idx)
		{
			luatable_0* t = new luatable_0(mapper.get_table_by_idx<luatable_0>(idx));
			tbl0s.insert(std::pair<string, luatable_0*>(t->c, t));
		});
	}
	void print()
	{
		std::for_each(tbl0s.begin(), tbl0s.end(), [](std::pair<string, luatable_0*>&& t)
		{
			t.second->print();
		});
	}
	std::map<string, luatable_0*> tbl0s;
};

int luatable_test()
{
	luastate_mgr::Instance().initialize();
	luatable_mapper mapper(filesystem::Instance().get_absolute_path("../luatable_0.lua"), luastate_mgr::Instance().getconfig_state());
	if (mapper.validate())
	{
		luatable_0 lt0;
		lt0.from_lua(mapper);
		lt0.print();

		luatable_mapper mapper1(filesystem::Instance().get_absolute_path("../luatable_1.lua"), luastate_mgr::Instance().getconfig_state());
		if (mapper1.validate())
		{
			luatable_1 lt1;
			lt1.from_lua(mapper1);
			lt1.print();
		}
		else
			return 1;
	}
	else
		return 1;
	return 0;
}
