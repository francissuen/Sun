#define gb_Interface class

#define SingletonDeclare(x) \
public:\
	static inline x& Instance()\
{\
	static x _instance; \
	return _instance; \
} \
private:\
	inline x(){};\
	inline x(x const&) {}\
	inline void operator = (x const&){}

#define SingletonDeclare_ExcludeDfnCnstrctor(x) \
public:\
	static inline x& Instance()\
{\
	static x _instance; \
	return _instance; \
} \
private:\
	inline void operator = (x const&){}

#define gbSAFE_DELETE_ARRAY(x) \
	if(x != nullptr)\
	{\
		delete [] x;\
		x = nullptr;\
	}

#define gbSAFE_DELETE(x)\
if(x != nullptr)\
{\
	delete x;\
	x = nullptr;\
}

template<typename From, typename To>
inline To* gb_safe_cast(From* from)
{
#ifdef GB_DEBUG
    To* to = dynamic_cast<To*>(from);
    assert(to != nullptr);
    return to;
#elif
    return static_cast<To*>(from);
#endif    
}

template<typename From, typename To>
inline To& gb_safe_cast(From& from)
{
#ifdef GB_DEBUG
    return dynamic_cast<To&>(from);
#elif
    return static_cast<To&>(from);
#endif    
}

    
