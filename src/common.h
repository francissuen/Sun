#pragma once
#include <cassert>

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

#define GB_REMOVE_PARENTHESE(...) __VA_ARGS__

#define _GB_EXCLUDE_FIRST_ARG_(first, ...) __VA_ARGS__
#define GB_EXCLUDE_FIRST_ARG(...) _GB_EXCLUDE_FIRST_ARG_(__VA_ARGS__)

#define _GB_GET32TH_ARGS_(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
		       _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
		       _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
		       _31, n, ...) n

#define GB_GET32TH_ARGS(...) _GB_GET32TH_ARGS_(__VA_ARGS__)

#define GB_GET32TH_ARGS_FROM_2ND(...) GB_GET32TH_ARGS(GB_EXCLUDE_FIRST_ARG(__VA_ARGS__))

#ifdef _MSC_VER
#define GB_SMART_GET32TH_FROM_COMMA__VA_ARGS__OTHER(other, ...) GB_GET32TH_ARGS_FROM_2ND \
    (_GB_COMMA__VA_ARGS__OTHER(other, __VA_ARGS__))
#elif defined(__GNUC__) || defined(__clang__)
#define GB_SMART_GET32TH_FROM_COMMA__VA_ARGS__OTHER(other, ...) GB_GET32TH_ARGS_FROM_2ND \
    (, ##__VA_ARGS__, other)

#endif

/**********************************/
// GB_SMART_COMMA


#define _GB_SMART_COMMA_ARGS_						\
  _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, \
  _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, \
  _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, \
  _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_COMMA_, _GB_EMPTY_


// #define aa cc,cc,
// #define ff(a, b, c) a
// #define fff(...) ff(__VA_ARGS__)
// int fff(aa) = 1;
// #define tt fff(aa)
// #define cc int
// tt x = 1;

#define _GB_SMART_COMMA_(...) GB_SMART_GET32TH_FROM_COMMA__VA_ARGS__OTHER \
  (_GB_SMART_COMMA_ARGS_, ##__VA_ARGS__)

// prevent _GB_COMMA_ expanding issue
#define _GB_COMMA_REAL_ ,

#define _GB_EMPTY_REAL_

#define GB_MERGE(a, b) a##b
#define _GB_SMART_COMMA_REAL_(a) GB_MERGE(a, REAL_)

#define GB_SMART_COMMA(...) _GB_SMART_COMMA_REAL_(_GB_SMART_COMMA_(__VA_ARGS__))

/**********************************/

/**********************************/
// GB_ARGC, support 0-31 args

#define _GB_ARGC_GET32TH_PRESET_ARGS_ 31, 30, 	\
	29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
	19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
	9, 8, 7, 6, 5, 4, 3, 2, 1, 0

// trailing comma suppressing
//https://msdn.microsoft.com/en-us/library/ms177415.aspx
//https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html 
#ifdef _MSC_VER
#define GB_ARGC(...)							\
    GB_SMART_GET32TH_FROM_COMMA__VA_ARGS__OTHER(_GB_ARGC_GET32TH_PRESET_ARGS_, __VA_ARGS__)
#elif defined(__GNUG__) || defined(__clang__)
#define GB_ARGC(...)							\
    GB_SMART_GET32TH_FROM_COMMA__VA_ARGS__OTHER(_GB_ARGC_GET32TH_PRESET_ARGS_, ##__VA_ARGS__)
#endif

static_assert( GB_ARGC() == 0, "GB_ARGC error 0");
static_assert( GB_ARGC(a) == 1, "GB_ARGC error 1");
static_assert( GB_ARGC(a, a) == 2, "GB_ARGC error 2");
static_assert(GB_ARGC(a, a, a, a, a, a, a, a, a, a,
 		      a, a, a, a, a, a, a, a, a, a,
		      a, a, a, a, a, a, a, a, a, a,
 		      a) == 31, "GB_ARGC error 31");

/**********************************/

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
