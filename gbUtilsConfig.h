//gbUtils config file

//version
#define gbUtils_VERSION_MAJOR 1
#define gbUtils_VERSION_MINOR 0


//export lua api if defined
/* #undef gbLUAAPI */

#define gbUtilsSTATIC

#ifdef gbUtilsSTATIC
#define gbUtilsEXPORT_CLASS class
#else
#ifdef gbUtilsEXPORT
#define gbUtilsEXPORT_CLASS class _declspec(dllexport)
#else
#define gbUtilsEXPORT_CLASS class _declspec(dllimport)
#endif
#endif

#ifdef gbUtilsSTATIC
#define gbUtilsEXPORT_STRUCT struct
#else
#ifdef gbUtilsEXPORT
#define gbUtilsEXPORT_STRUCT struct _declspec(dllexport)
#else
#define gbUtilsEXPORT_STRUCT struct _declspec(dllimport)
#endif
#endif
