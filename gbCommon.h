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

#define gbNEW(x, type)\
type* x = new type;	\
if(x == nullptr) \
	gbLog::Instance().Error(gbString("gbNEW") + #type);

#define gbNEW_ARRAY(x, type, count)\
type* x = new type[count];	\
if(x == nullptr) \
	gbLog::Instance().Error(gbString("gbNEW_ARRAY") + #type);




