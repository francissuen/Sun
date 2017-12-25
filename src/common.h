#pragma once

#define GB_SINGLETON(x)			\
    public:					\
    static inline x& Instance()			\
    {						\
	static x _instance;			\
	return _instance;			\
    }						\
private:					\
inline x(){};					\
inline x(x const&) {}				\
inline void operator = (x const&){}

#define GB_SINGLETON_EXCLUDECTOR(x) \
    public:					\
    static inline x& Instance()			\
    {						\
	static x _instance;			\
	return _instance;			\
    }						\
private:					\
x();						\
inline x(x const&) {}				\
inline void operator = (x const&){}

#define GB_FRIEND_BINARY_OPERATOR_DECLARE(return_t, operator_, operand_1_t, operand_2_t) \
    friend return_t operator operator_ (operand_1_t, operand_2_t); \
    friend return_t operator operator_ (operand_2_t, operand_1_t); \

//iff it's commutative
#define GB_FRIEND_BINARY_OPERATOR_DECLARE_SYMMETRICALLY(return_t, operator_, operand_1_t, operand_2_t) \
    friend return_t operator operator_ (operand_1_t, operand_2_t);	\
    inline friend return_t operator operator_ (operand_2_t o2, operand_1_t o1) \
    {									\
	return o1 operator_ o2;						\
    }

#define GB_SAFE_DELETE_ARRAY(x)			\
    if(x != nullptr)				\
    {						\
	delete [] x;				\
	x = nullptr;				\
    }

#define GB_SAFE_DELETE(x)			\
    if(x != nullptr)				\
    {						\
	delete x;				\
	x = nullptr;				\
    }

template<typename From, typename To>
inline To* gb_safe_cast(From* from)
{
#ifdef GB_DEBUG
    To* to = dynamic_cast<To*>(from);
    assert(to != nullptr);
    return to;
#else
    return static_cast<To*>(from);
#endif    
}

template<typename From, typename To>
inline To& gb_safe_cast(From& from)
{
#ifdef GB_DEBUG
    return dynamic_cast<To&>(from);
#else
    return static_cast<To&>(from);
#endif    
}
