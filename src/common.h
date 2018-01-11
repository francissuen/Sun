#pragma once
#include <cassert>
#include <iostream>

#define GB_SINGLETON(x)				\
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

#define GB_SINGLETON_EXCLUDECTOR(x)		\
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
    friend return_t operator operator_ (operand_1_t, operand_2_t);	\
    friend return_t operator operator_ (operand_2_t, operand_1_t);	\

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

#define GB_EXPAND(...) __VA_ARGS__
#define GB_MERGE(a, b) a##b
#define GB_CALL(func, param) func param

// using variadic macro for the type contains comma, such as std::map<int, int>
#define GB_PROPERTY_R(name, ...)		\
    private:					\
    __VA_ARGS__ _##name;			\
public:						\
inline const __VA_ARGS__ & Get##name()const	\
{						\
    return _##name;				\
}						\
inline __VA_ARGS__& Get##name()			\
{						\
    return _##name;				\
}

#define GB_PROPERTY_W(name, ...)		\
    private:					\
    __VA_ARGS__ _##name;			\
public:						\
inline void Set##name(const __VA_ARGS__& val)	\
{						\
    _##name = val;				\
}						\
inline void Set##name(__VA_ARGS__&& val)	\
{						\
    _##name = std::move(val);			\
}						


#define GB_PROPERTY(name, ...)			\
    private:					\
    __VA_ARGS__ _##name;			\
public:						\
inline const __VA_ARGS__& Get##name()const	\
{						\
    return _##name;				\
}						\
inline void Set##name(const __VA_ARGS__& val)	\
{						\
    _##name = val;				\
}						\
inline void Set##name(__VA_ARGS__&& val)	\
{						\
    _##name = std::move(val);			\
}						\
inline __VA_ARGS__& Get##name()			\
{						\
    return _##name;				\
}

#define _GB_EXCLUDE_FIRST_ARG_(first, ...) __VA_ARGS__
#ifdef _MSC_VER
//MS preprocessor issue. see https://stackoverflow.com/questions/48088834/how-to-implement-exclude-first-argument-macro-in-msvc
#define GB_EXCLUDE_FIRST_ARG(...) GB_CALL(_GB_EXCLUDE_FIRST_ARG_, (__VA_ARGS__))
#else
#define GB_EXCLUDE_FIRST_ARG(...) _GB_EXCLUDE_FIRST_ARG_(__VA_ARGS__)
#endif


#define _GB_GET32TH_ARGS_(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,	\
			  _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
			  _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
			  _31, n, ...) n

#ifdef _MSC_VER
#define GB_GET32TH_ARGS(...) GB_CALL(_GB_GET32TH_ARGS_, (__VA_ARGS__))
#else
#define GB_GET32TH_ARGS(...) _GB_GET32TH_ARGS_(__VA_ARGS__)
#endif


#define GB_GET32TH_ARGS_FROM_2ND(...) GB_GET32TH_ARGS(GB_EXCLUDE_FIRST_ARG(__VA_ARGS__))

// trailing comma suppressing
//https://msdn.microsoft.com/en-us/library/ms177415.aspx
//https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
#ifdef _MSC_VER
#define GB_COMMA__VA_ARGS__(...) , __VA_ARGS__
#elif defined(__GNUC__) || defined(__clang)
#define GB_COMMA__VA_ARGS__(...) , ##__VA_ARGS__
#endif

#define GB_SMART_GET32TH_FROM_COMMA__VA_ARGS__OTHER(other, ...) GB_GET32TH_ARGS_FROM_2ND \
    (GB_COMMA__VA_ARGS__(__VA_ARGS__), other)

/**********************************/
// GB__VA_ARGS__
#define _GB__VA_ARGS__PARAM_EMPTY_ 
#define _GB__VA_ARGS__PARAM_(param, empty)	param, param,	\
	param, param, param, param, param,			\
	param, param, param, param, param,			\
	param, param, param, param, param,			\
	param, param, param, param, param,			\
	param, param, param, param, param,			\
	param, param, param, param, empty			\

// expand to param  while count of __VA_ARGS__ greater than zero
#define GB__VA_ARGS__(param, ...) GB_SMART_GET32TH_FROM_COMMA__VA_ARGS__OTHER \
    (_GB__VA_ARGS__PARAM_(param, _GB__VA_ARGS__PARAM_EMPTY_), __VA_ARGS__)

// exception for comma(,)
#define _GB__VA_ARGS__COMMA_(...) GB_SMART_GET32TH_FROM_COMMA__VA_ARGS__OTHER \
    (_GB__VA_ARGS__PARAM_(_GB_COMMA_, _GB_COMMA_EMPTY_), __VA_ARGS__)

#define _GB_COMMA_REAL_ ,
#define _GB_COMMA_EMPTY_REAL_

#define _GB__VA_ARGS__COMMA_MERGE_(a) GB_MERGE(a, REAL_)

#ifdef _MSC_VER
#define _GB__VA_ARGS__COMMA_MERGE_MSC_WRAPPER_(a) _GB__VA_ARGS__COMMA_MERGE_(a)
#define GB__VA_ARGS__COMMA(...) _GB__VA_ARGS__COMMA_MERGE_MSC_WRAPPER_	\
    (_GB__VA_ARGS__COMMA_(__VA_ARGS__)) 
#elif
#define GB__VA_ARGS__COMMA(...) _GB__VA_ARGS__COMMA_MERGE_	\
    (_GB__VA_ARGS__COMMA_(__VA_ARGS__)) 
#endif
/**********************************/

/**********************************/
// GB_ARGC, support 0-31 args

#define _GB_ARGC_GET32TH_PRESET_ARGS_ 31, 30, 	\
	29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
	19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
	9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define GB_ARGC(...)     GB_SMART_GET32TH_FROM_COMMA__VA_ARGS__OTHER	\
    (_GB_ARGC_GET32TH_PRESET_ARGS_, __VA_ARGS__)

static_assert( GB_ARGC() == 0, "GB_ARGC error 0");
static_assert( GB_ARGC(a) == 1, "GB_ARGC error 1");
static_assert( GB_ARGC(a, a) == 2, "GB_ARGC error 2");
static_assert(GB_ARGC(a, a, a, a, a, a, a, a, a, a,
 		      a, a, a, a, a, a, a, a, a, a,
		      a, a, a, a, a, a, a, a, a, a,
 		      a) == 31, "GB_ARGC error 31");

/**********************************/
#ifdef NDEBUG
#define GB_ASSERT(condition, ...) 
#else

#define GB_ASSERT(condition, ...)					\
    static_assert(GB_ARGC(__VA_ARGS__) == 0 || GB_ARGC(__VA_ARGS__) == 1, \
		  "GB_ASSERT can take only zero or one MSG arg");	\
    if(!(condition))							\
    {									\
     std::cerr << "**************** GB_ASSERT FAILED ****************"	\
	       << std::endl;						\
     std::cerr << "CONDITION@ " << #condition << std::endl;		\
     std::cerr << "LINE@ " << __LINE__ << std::endl			\
	       << "FILE@ " << __FILE__ << std::endl;			\
     std::cerr << "MSG@ " GB__VA_ARGS__(<< ,__VA_ARGS__)  __VA_ARGS__	\
	       << std::endl;						\
     std::cerr << "**************** GB_ASSERT FAILED ****************"	\
	       << std::endl;						\
     assert(false);							\
    }
#endif

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
