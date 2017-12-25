#pragma once

#include <cassert>
#include <unordered_map>
#include <vector>
#include "string.h"

GB_UTILS_NS_BEGIN

/*
 *supported arguments formats
 *[-opt [opt_arg]]... [arg]...
 *[opt_arg] called named arg, 
 *[arg] called unnamed arg(always after named args, except argv[0])
 *note! unnamed_arg(0) always return current executable path
 */
GB_UTILS_CLASS args
{
public:
    enum enmType
    {
	DEFAULT = 0,
	BOOL, INT, STRING, UNNAMED
    };
private:
    struct _namedArg
    {
	inline _namedArg(const char opt_, const enmType type_):
	    opt(opt_),
	    type(type_),
	    _rawArg(0),
	    _isSupplied(false)
	    {}
	const char opt;
	const enmType type;
	void SetArg(const std::uintptr_t arg_);
	inline std::uintptr_t GetArg()const
	    {
		return _rawArg;
	    }
	inline bool IsSupplied()const{ return _isSupplied; }
    private:
	std::uintptr_t _rawArg;
	bool _isSupplied;
    };
public:
    inline args(){}
    ~args();
    void register_namedArg(const char opt, const enmType type);
    void parse(const int argc, char** argv);
    template<typename T>
	T named_arg(const char opt)const;
    bool has_named_arg(const char opt)const;
    const char* unnamed_arg(const unsigned int idx)const;
    inline size_t unnamed_arg_size()const
    {
	return _vUnnamedArgs.size();
    }
private:
    typedef std::unordered_map<char, _namedArg*> mpNamedArg_t;
    std::unordered_map<char, _namedArg*> _mpNamedArgs;
	  
    std::vector<char*> _vUnnamedArgs;
};

GB_UTILS_NS_END
