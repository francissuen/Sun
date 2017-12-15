#pragma once
#include "config.h"
#include "ns.h"
/*
 *@brief, for concurrent paring
 terms: 1.table, which can contain elements,
        2.element, which also can be a table, stored in orded

possible features

1. table::parse
       tasklist = concurrency new tasklist
       foreach element in table
               if element not a table
	          fetch value
               else
	          tasklist.psuh(table::parse(element))// a new table
      tasklist.done()

 */


GB_UTILS_NS_BEGIN

class t0
{
    int a;
    int b;

    void Serialize(file);
};

class ele_info
{
    ele_info():_IsTable(false)
	{}
    void* addr;
    size_t size;
    bool _IsTable; 
    ser(file)
	{
	    if(!_IsTable)
		file.write(addr, size);
	    else
		reinterpret_cast<cpp_table*>(addr)->Serialize(file);
	}
}

class t1
{
    int a;
    char b;
    char* c;
    t0 d;
    short e[10];
    // std::list<int> f;
    // std::list<t0> g;
    
    std::vector<ele_info> _eles;

    void Initialize_eles()
	{
	    _eles.push(&a, sizeof(int));
	    _eles.push(&b, sizeof(b));
	    if(c != nullptr)
		_eles.push(c, strlen(c) + 1);
	    _else.push(e, sizeof(e));
	    _else.push(f, 0, true);
	    _else.push(g, 0, true);
	}
    void serialize(file)
	{
	    std::for_each(_els.begin(), _els.end(), [](ele_info& ele)
			  {
			      
			  })
	}
};


GB_UTILS_CLASS cpp_table
{
    void serialize(file);
};

GB_UTILS_NS_END
