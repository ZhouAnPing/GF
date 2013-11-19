
#ifdef _STP_NOT_WINDOWS
typedef const char * LPCSTR;
#else
typedef int socklen_t;
#endif

#ifdef _STP_NEED___INT64
typedef long long __int64;
typedef unsigned long long __uint64;
#else
typedef unsigned __int64 __uint64;
#endif

#ifdef _STP_VMS_ALPHA
#define __USE_STD_IOSTREAM
typedef size_t socklen_t;
#endif


#ifdef _STP_VMS_ITANIUM
typedef size_t socklen_t;
#endif



#ifdef _STP_GNU_COMPILER
#define TYPENAME typename
#else
#define TYPENAME 
#endif

#ifdef _STP_NOT_WINDOWS
typedef int SOCKET;
typedef unsigned long DWORD;
typedef void * ThreadReturnType;
#else
typedef unsigned int ThreadReturnType;
#define swab(x, y, z) _swab(x, y, z)
#endif

#define STP_USER_THREADPROC(x) ThreadReturnType x(void * arg)
