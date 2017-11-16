#include "args.h"
#include <cstdlib>
#include "string.h"
using gb::utils::args;
using gb::utils::string;

args::~args()
{
    for(mpNamedArg_t::iterator i =  _mpNamedArgs.begin(); i != _mpNamedArgs.end(); i++)
    {
	delete i->second;
    }
}

void args::_namedArg::SetArg(const std::uintptr_t arg_)
{
    _rawArg = arg_;
    _isSupplied = true;
}

void args::register_namedArg(const char opt, const enmType type)
{
    _namedArg* na = new _namedArg(opt, type);
    _mpNamedArgs.insert(std::pair<char, _namedArg*>(opt, na));
}


void args::parse(const int argc, char** argv)
{
    assert(argv != nullptr);
    
    _namedArg* curNamedArg = nullptr;
    enmType curType = enmType::DEFAULT;
    char curOpt = -1;
    for(int i = 0; i < argc; i++)
    {
	char* arg = argv[i];
	assert(arg != nullptr);
	if(curType == enmType::BOOL || curType == enmType::DEFAULT)
	{
	    if(curType == enmType::BOOL)
		curNamedArg->SetArg(1);

	    //searching for next opt
	    int j = 0;
	    if(arg[j++] == '-')
	    {
		curOpt = arg[j];
		if(curOpt == '\0')
		    throw string("unexpected '\\0' after '-' when expecting a opt");

		mpNamedArg_t::iterator i = _mpNamedArgs.find(curOpt);
		if(i == _mpNamedArgs.end())
		    throw string("unknown opt: ") + curOpt;

		curNamedArg = i->second;
		curType = curNamedArg->type;
	    }
	    else//unnamed args
	    {
		if(*arg == '-')
		    throw string("unexpected '-' when expecting a arg");
			    
		_vUnnamedArgs.push_back(arg);
		if(i != 0)//argv[0] always is current executable path
		    curType = enmType::UNNAMED;
	    }
	}
	else//arg
	{
	    if(*arg == '-')
		throw string("unexpected '-' when expecting a arg");
	    if(curType == enmType::UNNAMED)
		_vUnnamedArgs.push_back(arg);
	    else if(curType == enmType::INT)
	    {
		int intArg = ::atoi(arg);
		curNamedArg->SetArg(intArg);
		curType = enmType::DEFAULT;
	    }
	    else if(curType == enmType::STRING)
	    {
		curNamedArg->SetArg((std::uintptr_t)arg);
		curType = enmType::DEFAULT;
	    }
	}
    }
}

template<typename T>
T args::named_arg(const char opt)const
{
    mpNamedArg_t::const_iterator i = _mpNamedArgs.find(opt);
    if(i == _mpNamedArgs.end())
	throw string("unknown opt: ") + opt;
    
    return (T)(i->second->GetArg());
}

bool args::has_named_arg(const char opt)const
{
    mpNamedArg_t::const_iterator i = _mpNamedArgs.find(opt);
    if(i == _mpNamedArgs.end())
	return false;
    else
	return i->second->IsSupplied();
}
template bool args::named_arg<bool>(const char opt)const;
template int args::named_arg<int>(const char opt)const;
template char* args::named_arg<char*>(const char opt)const;

const char* args::unnamed_arg(const unsigned int idx)const
{
    if(idx >= _vUnnamedArgs.size())
	throw string("idx >= _vUnnamedargs idx:") + idx;
    return _vUnnamedArgs[idx];
}
